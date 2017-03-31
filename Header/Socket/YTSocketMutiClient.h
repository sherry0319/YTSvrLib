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
#pragma once
#ifndef __YTSOCKET_MUTI_CLIENT_H_
#define __YTSOCKET_MUTI_CLIENT_H_

namespace YTSvrLib
{
	class ITCPMUTICLIENTCONTROLLER;
	class ITCPMUTICLIENT : public ITCPBASE
	{
	public:
		ITCPMUTICLIENT()
		{
			m_bIsConnecting = FALSE;
			m_bIsConnected = FALSE;
			m_bIsDisconnecting = FALSE;
			m_pController = NULL;
		}

		virtual ~ITCPMUTICLIENT()
		{

		}

		virtual void OnError(int nErrCode);

		virtual void OnConnected(){}

		virtual void SafeClose();

		virtual void OnClosed() = 0;

		virtual void OnDisconnecting();

		virtual BOOL CreateClient(ITCPMUTICLIENTCONTROLLER* pController,const char* pszIP, int nPort);

		virtual BOOL IsDisconnecting()
		{
			return m_bIsDisconnecting;
		}
	protected:
		virtual void Clean()
		{
			ITCPBASE::Clean();
			m_bIsConnecting = FALSE;
			m_bIsConnected = FALSE;
			m_bIsDisconnecting = FALSE;
		}
	protected:
		BOOL m_bIsConnecting;
		BOOL m_bIsConnected;
		BOOL m_bIsDisconnecting;
		ITCPMUTICLIENTCONTROLLER* m_pController;
	};
}

#endif // !__YTSOCKET_MUTI_CLIENT_H_
