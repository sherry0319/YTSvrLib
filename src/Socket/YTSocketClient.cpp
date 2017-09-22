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
#include "YTSocketClient.h"

namespace YTSvrLib
{
	void ITCPCLIENT::OnError(int nErrCode)
	{
		LOG("ITCPCLIENT::OnError : %d",nErrCode);

		SafeClose();
	}

	void ITCPCLIENT::SafeClose()
	{
		if (m_bIsClosed)
		{
			return;
		}

		m_bIsConnected = FALSE;
		m_bIsClosed = TRUE;
		m_bIsConnecting = FALSE;
		m_bIsDisconnecting = FALSE;

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

		OnClosed();
	}

	void ITCPCLIENT::ReleaseClient()
	{
		StopThread();
	}

	void ITCPCLIENT::OnThreadEnd()
	{
		SafeClose();

		ITCPEVENTTHREAD::CleanThread();
	}

	void ITCPCLIENT::OnDisconnecting()
	{
		m_bIsDisconnecting = TRUE;

		PostDisconnectMsg(eDisconnect);
	}

	BOOL ITCPCLIENT::CreateClient(const char* pszIP, int nPort)
	{
		if (m_bIsConnecting)
		{
			return FALSE;
		}

		if (GetEvent() == NULL)
		{
			if (CreateEvent() == FALSE)
			{
				return FALSE;
			}
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
			m_pbufferevent = bufferevent_socket_new(GetEvent(), GetSocket() , BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);

			bufferevent_setwatermark(m_pbufferevent, EV_READ, 0, 0);

			bufferevent_setcb(m_pbufferevent, OnRead, NULL, ITCPBASE::OnError, this);
		}

		if (bufferevent_socket_connect(m_pbufferevent, (sockaddr*) &sAddrDst, sizeof(sAddrDst)))
		{
			LOGERROR("Error : bufferevent_socket_connect failed : %d", GetLastError());
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

		m_bIsClosed = FALSE;

		if (IsRuning() == FALSE)
		{
			CreateThread();
		}

		bufferevent_enable(m_pbufferevent, EV_READ | EV_SIGNAL | EV_PERSIST);

		return true;
	}
}