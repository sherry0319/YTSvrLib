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
#include "YTSocketServer.h"

namespace YTSvrLib
{
	BOOL ITCPSERVER::StartListen(int nPort, const char* pszIP)
	{
		if (CreateEvent() == FALSE)
		{
			return FALSE;
		}

		if (m_listener)
		{
			StopListen();
		}

		sockaddr_in sBind;
		ZeroMemory(&sBind, sizeof(sBind));
		sBind.sin_family = AF_INET;
		sBind.sin_port = htons((u_short) nPort);
#ifdef LIB_WINDOWS
		if (pszIP == NULL || strlen(pszIP) == 0 || (strcmp(pszIP, "0.0.0.0") == 0))
		{
			sBind.sin_addr.S_un.S_addr = INADDR_ANY;
		}
		else
		{
			sBind.sin_addr.S_un.S_addr = inet_addr(pszIP);
		}
#else
		if (pszIP == NULL || (strcmp(pszIP, "0.0.0.0") == 0))
		{
			sBind.sin_addr.s_addr = htonl(INADDR_ANY);
		}
		else
		{
			sBind.sin_addr.s_addr = inet_addr(pszIP);
		}
#endif
		m_listener = evconnlistener_new_bind(GetEvent(), OnAccept, this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE, -1, (sockaddr*) &sBind, sizeof(sBind));
		if (m_listener == NULL)
		{
			LOG("libevent listener init failed!! : %d",GetLastError());
			return FALSE;
		}

		m_fd = evconnlistener_get_fd(m_listener);

		CreateThread();

		return TRUE;
	}

	BOOL ITCPSERVER::IsListening()
	{
		if (m_listener)
		{
			return TRUE;
		}
		return FALSE;
	}

	void ITCPSERVER::StopListen()
	{
		StopThread();
	}

	void ITCPSERVER::OnThreadEnd()
	{
		if (m_listener)
		{
			evconnlistener_free(m_listener);
			m_listener = NULL;
		}

		if (m_fd)
		{
			closesocket(m_fd);
			m_fd = 0;
		}

		OnServerClose();
	}

	void ITCPSERVER::OnAccept(struct evconnlistener *, evutil_socket_t fd, struct sockaddr *sock, int, void *arg)
	{
		LOG("OnAccept Success : Sock=%d", fd);

		ITCPSERVER* pTCPServer = (ITCPSERVER*) arg;

		if (pTCPServer)
		{
			ITCPCONNECTOR* pConnector = pTCPServer->AllocateConnector();
			if (pConnector == NULL)
			{
				LOG("Allocate ITCPCONNECTOR Failed");
				return;
			}
			sockaddr_in* sockin = (sockaddr_in*) sock;
			int nPort = (int) ntohs(sockin->sin_port);
			char* szIP = inet_ntoa(sockin->sin_addr);

			LOG("OnAccept Create Client Success : %s:%d On Sock=%d", szIP, nPort, fd);

			if (pConnector->CreateConnector(pTCPServer, (SOCKET) fd, szIP, nPort) == FALSE)
			{
				pTCPServer->ReleaseConnector(pConnector);
			}
		}
	}
}