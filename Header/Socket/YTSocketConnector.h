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
#ifndef __YTSOCKET_CONNECTOR_H_
#define __YTSOCKET_CONNECTOR_H_

#include "YTSocketBase.h"

namespace YTSvrLib
{
	class ITCPSERVER;
	class YTSVRLIB_EXPORT ITCPCONNECTOR : public ITCPBASE
	{
	public:
		ITCPCONNECTOR()
		{
			Clean();
		}

		virtual ~ITCPCONNECTOR()
		{

		}

		virtual void Clean()
		{
			ITCPBASE::Clean();
			m_pTCPServer = NULL;
			m_bIsDisconnecting = FALSE;
		}

		virtual void OnError(int nErrCode) override;

		virtual void SafeClose();

		virtual void OnDisconnect() = 0;

		virtual void OnClosed() = 0;

		virtual void ReclaimObj() = 0;

		virtual BOOL CreateConnector(ITCPSERVER* pTCPServer, SOCKET fd, const char* pszHost, int nPort);

		virtual void OnDisconnecting();

		virtual BOOL IsDisconnecting()
		{
			return m_bIsDisconnecting;
		}
	public:
		ITCPSERVER* m_pTCPServer;// π‹¿Ì’ﬂ
		BOOL m_bIsDisconnecting;
		CLock m_lock;
	};
}

#endif // !__YTSOCKET_CONNECTOR_H_
