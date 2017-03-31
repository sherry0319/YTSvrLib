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
#ifndef __YTSOCKETSERVER_H_
#define __YTSOCKETSERVER_H_

#include <event.h>
#include <event2/listener.h>

#include "YTSocketThread.h"
#include "YTSocketConnector.h"

namespace YTSvrLib
{
	class ITCPSERVER : public ITCPEVENTTHREAD
	{
	public:
		ITCPSERVER()
		{
			Clean();
		}

		virtual ~ITCPSERVER()
		{

		}

		virtual void Clean()
		{
			ITCPEVENTTHREAD::CleanThread();
			m_listener = NULL;
			m_nListenPort = 0;
			m_fd = 0;
		}

		BOOL StartListen(int nPort, const char* pszIP);

		void StopListen();

		virtual ITCPCONNECTOR* AllocateConnector() = 0;

		virtual void ReleaseConnector(ITCPCONNECTOR* pConnect) = 0;

		static void OnAccept(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sock, int socklen, void *arg);

		virtual void OnThreadEnd();

		virtual void OnServerClose() = 0;
	protected:
		evconnlistener* m_listener;
		int m_nListenPort;
		SOCKET m_fd;
	};
}

#endif // !__YTSOCKETSERVER_H_
