/*MIT License

Copyright (c) 2016 Zhe Xu

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
#include "YTSocketConnector.h"
#include "YTSocketServer.h"

namespace YTSvrLib
{
	BOOL ITCPCONNECTOR::CreateConnector(ITCPSERVER* pTCPServer, SOCKET fd, const char* pszHost, int nPort)
	{
		if (pTCPServer == NULL)
		{
			return FALSE;
		}

		event_base* eventbase = pTCPServer->GetEvent();

		if (eventbase == NULL)
		{
			return FALSE;
		}

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

		m_fd = fd;
		strncpy_s(m_szHost, pszHost, 31);
		m_nPort = nPort;

		m_pbufferevent = bufferevent_socket_new(eventbase, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
		if (m_pbufferevent == NULL)
		{
			LOGERROR("bufferevent_socket_new failed : %d",GetLastError());
			return FALSE;
		}

		bufferevent_setwatermark(m_pbufferevent, EV_READ, 0, 0);
		bufferevent_setcb(m_pbufferevent, OnRead, NULL, ITCPBASE::OnError, this);
		bufferevent_enable(m_pbufferevent, EV_READ | EV_SIGNAL | EV_PERSIST);

		m_pTCPServer = pTCPServer;

		LOG("Socket=%d Accepted Remote=%s:%d", GetSocket(), GetAddrIp(), GetAddrPort());

		m_bIsClosed = FALSE;

		PostDisconnectMsg(eAccepted);// 给主线程投递Accept成功的消息

		return TRUE;
	}

	void ITCPCONNECTOR::OnError(int nErrCode)
	{
		LOG("ITCPCONNECTOR::OnError : %d",nErrCode);
		SafeClose();
	}

	void ITCPCONNECTOR::SafeClose()
	{
		if (m_bIsDisconnecting)
		{
			return;
		}
		m_bIsDisconnecting = TRUE;
		LOG("ITCPCONNECTOR::SafeClose[0x%x] = %s:%d @ Socket(%d)",this, GetAddrIp(), GetAddrPort(), GetSocket());

		m_bIsClosed = TRUE;

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

		OnClosed();

		m_bIsDisconnecting = TRUE;

		ReclaimObj();
	}
}

