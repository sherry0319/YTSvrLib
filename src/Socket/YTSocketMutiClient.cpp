/*MIT License

Copyright (c) 2016 Archer Xu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#include "stdafx.h"
#include "YTSocketMutiClient.h"
#include "YTSocketMutiClientController.h"

namespace YTSvrLib
{

	void ITCPMUTICLIENT::OnError(int nErrCode)
	{
		LOG("ITCPMUTICLIENT::OnError : %d", nErrCode);

		SafeClose();
	}

	void ITCPMUTICLIENT::SafeClose()
	{
		if (m_bIsDisconnecting)
		{
			return;
		}

		m_lockBufferEvent.Lock();

		m_bIsDisconnecting = TRUE;
		m_bIsConnected = FALSE;
		m_bIsClosed = TRUE;
		m_bIsConnecting = FALSE;

		if (m_pbufferevent)
		{
			bufferevent_disable(m_pbufferevent, EV_READ | EV_SIGNAL | EV_PERSIST);
			bufferevent_free(m_pbufferevent);
			m_pbufferevent = NULL;
		}

		if (m_fd)
		{
			closesocket(m_fd);
			m_fd = 0;
		}

		m_bIsDisconnecting = FALSE;

		m_lockBufferEvent.UnLock();
	}

	void ITCPMUTICLIENT::OnDisconnecting()
	{
		m_lockBufferEvent.Lock();

		if (m_pbufferevent)
		{
			bufferevent_disable(m_pbufferevent, EV_READ | EV_SIGNAL | EV_PERSIST);
			bufferevent_free(m_pbufferevent);
			m_pbufferevent = NULL;
		}

		if (m_fd)
		{
			closesocket(m_fd);
			m_fd = 0;
		}

		m_lockBufferEvent.UnLock();

		PostDisconnectMsg(eDisconnect);
	}

	BOOL ITCPMUTICLIENT::CreateClient(ITCPMUTICLIENTCONTROLLER* pController, const char* pszIP, int nPort)
	{
		if (m_bIsConnecting)
		{
			return FALSE;
		}

		if (pController == NULL)
		{
			LOG("pController == NULL : Are you kidding me?");
			return FALSE;
		}

		if (pController->CreateEvent() == FALSE)
		{
			return FALSE;
		}

		m_bIsConnecting = TRUE;

		if (m_fd == 0 || m_fd == INVALID_SOCKET)
		{
			if (CreateAsyncClientSock() == FALSE)
			{
				m_bIsConnecting = FALSE;
				return FALSE;
			}

			m_nPort = nPort;
			strncpy_s(m_szHost, pszIP, 31);
		}
		
		sockaddr_in sAddrDst;
#ifdef LIB_WINDOWS
		sAddrDst.sin_family = AF_INET;
		sAddrDst.sin_addr.S_un.S_addr = inet_addr(pszIP);
		sAddrDst.sin_port = htons((u_short) nPort);
#else
		sAddrDst.sin_family = AF_INET;
		sAddrDst.sin_addr.s_addr = inet_addr(pszIP);
		sAddrDst.sin_port = htons((u_short) nPort);
#endif // LIB_WINDOWS

		if (m_pbufferevent == NULL)
		{
			// BEV_OPT_THREADSAFE 不打的话OnDisconnecting崩溃
			m_pbufferevent = bufferevent_socket_new(pController->GetEvent(),GetSocket() , BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);

			bufferevent_setwatermark(m_pbufferevent, EV_READ, 0, 0);

			bufferevent_setcb(m_pbufferevent, OnRead, NULL, ITCPBASE::OnError, this);
		}

		if (m_pbufferevent && bufferevent_socket_connect(m_pbufferevent, (sockaddr*) &sAddrDst, sizeof(sAddrDst)))
		{
			OnError(GetLastError());
			if (m_pbufferevent)
			{// Don't be too naive...bufferevent_socket_connect里面可能会先触发回调导致m_pbufferevent被删除
				bufferevent_free(m_pbufferevent);
				m_pbufferevent = NULL;
			}
			if (m_fd)
			{
				closesocket(GetSocket());
				m_fd = 0;
			}
			m_bIsConnecting = FALSE;
			return FALSE;
		}

		bufferevent_enable(m_pbufferevent, EV_READ | EV_SIGNAL | EV_PERSIST);

		m_pController = pController;

		pController->CreateThread();

		m_bIsClosed = FALSE;

		return TRUE;
	}
}