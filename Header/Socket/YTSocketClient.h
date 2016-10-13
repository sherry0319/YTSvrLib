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
#ifndef __YTSOCKETCLIENT_H__
#define __YTSOCKETCLIENT_H__

#include "YTSocketBase.h"
#include "YTSocketThread.h"

namespace YTSvrLib
{
	class ITCPCLIENT : public ITCPBASE, public ITCPEVENTTHREAD
	{
	public:
		ITCPCLIENT()
		{
			Clean();
		}

		virtual ~ITCPCLIENT()
		{
			
		}
	public:
		virtual void OnConnected(){}

		virtual void OnError(int nErrCode);

		virtual void SafeClose();

		void ReleaseClient();

		virtual void OnClosed() = 0;

		virtual void OnDisconnecting();

		virtual void OnThreadEnd();

		virtual BOOL CreateClient(const char* pszIP, int nPort);

		virtual BOOL IsDisconnecting()
		{
			return m_bIsDisconnecting;
		}
	protected:
		virtual void Clean()
		{
			ITCPBASE::Clean();
			ITCPEVENTTHREAD::CleanThread();
			m_bIsConnecting = FALSE;
			m_bIsConnected = FALSE;
			m_bIsDisconnecting = FALSE;
		}
	protected:
		BOOL m_bIsConnecting;
		BOOL m_bIsConnected;
		BOOL m_bIsDisconnecting;
		YTSvrLib::CLock m_lockBufferEvent;
	};
}

#endif // !__YTSOCKETCLIENT_H__
