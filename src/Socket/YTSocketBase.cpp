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
#include "YTSocketBase.h"
#ifndef LIB_WINDOWS
#include <fcntl.h>
#include <linux/tcp.h>
#endif // !LIB_WINDOWS

namespace YTSvrLib
{
	void ITCPBASE::OnRead(bufferevent *, void *arg)
	{
		ITCPBASE* pSocket = (ITCPBASE*) arg;
		if (pSocket == NULL)
		{
			LOG("ITCPBASE::OnRead Error : pSocket == NULL");
			return;
		}

		pSocket->OnEventRecvHandle();
	}

	void ITCPBASE::OnDisconnecting()
	{
		PostDisconnectMsg(eDisconnect);
	}

	void ITCPBASE::OnError(bufferevent *, short events, void *arg)
	{
		ITCPBASE* pSocket = (ITCPBASE*) arg;
		if (pSocket == NULL)
		{
			LOG("ITCPBASE::OnError Failed : pSocket == NULL");
			return;
		}

		if (events & BEV_EVENT_CONNECTED)
		{// 这个不是错误.是异步连接成功的回调
			LOG("0x%x On Connected", pSocket);
			pSocket->OnConnected();
		}
		else if ( (events & BEV_EVENT_ERROR) || 
				  (events & BEV_EVENT_EOF) )
		{
			LOG("0x%x On Error : %d", pSocket,GetLastError());
			pSocket->OnDisconnecting();
		}
	}

	void ITCPBASE::OnEventRecvHandle()
	{
		evbuffer* input = bufferevent_get_input(m_pbufferevent);
		size_t nSizeRead = evbuffer_get_length(input);

		if (nSizeRead > 0)
		{
			int nRecved = OnSocketRecv();
			if (nRecved < 0)
			{
				LOG("OnSocketRecv Error : SizeRead : %d nRecved : %d",nSizeRead,nRecved);
				return;
			}
			
			nSizeRead -= nRecved;
		}

		size_t nRead = OnRecved(m_recvBuf.GetBuffer(), (int) m_recvBuf.GetLength());

		if (nRead == 0 && m_recvBuf.GetLength() >= (int) m_recvBuf.GetCapacity()
			&& m_recvBuf.GetCapacity() < m_recvBuf.GetBufSizeMax())
		{
			size_t nNewBufSize = m_recvBuf.GetCapacity() + BLOCK_RECV_BUFFER_SIZE;
			if (nNewBufSize > m_recvBuf.GetBufSizeMax())
				nNewBufSize = m_recvBuf.GetBufSizeMax();
			m_recvBuf.ReSize(nNewBufSize);
		}

		m_recvBuf.ReleaseBuffer(nRead);

		if (nSizeRead > 0)
		{
			OnEventRecvHandle();
		}
	}

	int ITCPBASE::OnSocketRecv()
	{
		if (m_pbufferevent == NULL)
		{
			return -1;
		}

		char* buf = m_recvBuf.GetBuffer() + m_recvBuf.GetLength();
		int nMaxLen = (int) (m_recvBuf.GetCapacity() - m_recvBuf.GetLength());

		size_t nRealRead = bufferevent_read(m_pbufferevent, buf, nMaxLen);

		if (nRealRead == 0)
		{
			SafeClose();
			return -1;
		}

		m_recvBuf.AddBuffer(nRealRead);

		return (int) nRealRead;
	}

	void ITCPBASE::Send(const char* info, int len)
	{
		if (len <= 0 || !info || m_bIsClosed || IsDisconnecting())
			return;

		BOOL bSend = FALSE;
		BOOL bError = FALSE; //是否有错误发生

		m_sendLock.Lock();
		try
		{
			if (m_sendBuf.IsSending() == FALSE)
			{
				if (m_sendBuf.AddBuffer(info, len))
				{
					m_bIsSending = TRUE;
					bSend = TRUE;
				}
				else
				{
					bError = TRUE;
				}
			}
			else
			{
				if (m_sendBuf.AddBuffer(info, len) == FALSE)
				{
					bError = TRUE;
				}
			}
		}
		catch (...)
		{
			m_sendLock.UnLock();
			return;
		}

		m_sendLock.UnLock();

		if (bSend)
		{
			OnSend();
		}

		if (bError)
		{
			LOG("Send AddBuffer Error");
			SafeClose();
		}
	}

	void ITCPBASE::OnSend()
	{
		if (m_bIsClosed || IsDisconnecting())
		{
			return;
		}
		m_bIsSending = TRUE;

		char* buf = m_sendBuf.GetDataToSend();
		int nLen = m_sendBuf.GetDataLenToSend();
		int nSend = 0;

		while (buf)
		{
			if ((nSend = send(GetSocket(), buf, nLen,0)) >= 0)
			{
				// LOG("Data Send = %d/%d",nSend,nLen);
				m_sendLock.Lock();
				BOOL bHasDataToSend = m_sendBuf.OnSend(nSend);
				m_sendLock.UnLock();

				if (m_bIsClosed || IsDisconnecting())
				{
					return;
				}
				
				if (bHasDataToSend)
				{
					buf = m_sendBuf.GetDataToSend();
					nLen = m_sendBuf.GetDataLenToSend();
				}
				else
				{
					break;
				}
			}
			else
			{
				DWORD dwCode = GetLastError();
				
#ifdef LIB_WINDOWS
				if (dwCode != WSAEWOULDBLOCK)
#else
				if (dwCode != ENOMEM && dwCode != EAGAIN)
#endif // LIB_WINDOWS
				{
					LOG("Send Data Error : %d.Error : %d", nSend, dwCode);
					SafeClose();
					break;
				}
#ifdef LIB_WINDOWS
				if (dwCode == WSAEWOULDBLOCK)
#else
				if (dwCode == EAGAIN)
#endif
				{// 如果这里返回EAGAIN.这是一个特殊情况.说明在非阻塞socket中send太快了已经占满了系统写缓冲区.要等待系统处理完系统写缓冲区之后再进行处理.
					// 所以强制调用一次select.监视写缓冲区.如果写缓冲区可写了.则继续写.如果超时或者错误了.则直接返回.
					fd_set fds;
					timeval timeout = { 1, 0 };// 设置1秒超时.如果1秒还没处理完也太慢了吧

					FD_ZERO(&fds);
					FD_SET(GetSocket(), &fds);

					int ret = select((int)GetSocket()+1,NULL,&fds,NULL,&timeout);
					if (ret <= 0)
					{
						SafeClose();
						break;
					}
				}
			}
		}

		m_bIsSending = FALSE;
	}

	BOOL ITCPBASE::CreateAsyncClientSock()
	{
		if (m_fd != 0 && m_fd != INVALID_SOCKET)
		{
			return TRUE;
		}

		SOCKET nSock = socket(AF_INET, SOCK_STREAM, 0);
		if (nSock == 0 || nSock == INVALID_SOCKET)
		{
			LOGERROR("Error : socket failed : %d", GetLastError());
			return FALSE;
		}

		m_fd = nSock;

		sockaddr_in sAddrMy;
#ifdef LIB_WINDOWS
		sAddrMy.sin_family = AF_INET;
		sAddrMy.sin_addr.S_un.S_addr = INADDR_ANY;
		sAddrMy.sin_port = 0;

		u_long flag = 1;
		int nResult = ioctlsocket(nSock, FIONBIO, &flag);
		if (nResult != NO_ERROR)
		{
			LOGERROR("Error : ioctlsocket failed : %d", GetLastError());
			OnError(GetLastError());
			closesocket(nSock);
			m_fd = 0;
			return FALSE;
		}

		BOOL one = TRUE;
		setsockopt(nSock, SOL_SOCKET, SO_REUSEADDR, (const char*) &one, sizeof(one));

		one = TRUE;
		setsockopt(nSock, SOL_SOCKET, SO_KEEPALIVE, (const char*) &one, sizeof(one));
#else
		sAddrMy.sin_family = AF_INET;
		sAddrMy.sin_addr.s_addr = INADDR_ANY;
		sAddrMy.sin_port = 0;

		int flags = 0;
		if ((flags = fcntl(nSock, F_GETFL)) == -1)
		{
			LOGERROR("Error : fcntl 1 failed : %d", GetLastError());
			OnError(GetLastError());
			closesocket(nSock);
			m_fd = 0;
			return FALSE;
		}

		flags |= O_NONBLOCK;

		if (fcntl(nSock, F_SETFL, flags) == -1)
		{
			LOGERROR("Error : fcntl 2 failed : %d", GetLastError());
			OnError(GetLastError());
			closesocket(nSock);
			m_fd = 0;
			return FALSE;
		}

		int one = 1;
		setsockopt(nSock, SOL_SOCKET, SO_REUSEADDR, (void*) &one, sizeof(one));

		setsockopt(nSock, IPPROTO_TCP, TCP_NODELAY, (void*) &one, sizeof(one));

		setsockopt(nSock, SOL_SOCKET, SO_KEEPALIVE, (void*) &one, sizeof(one));
#endif // LIB_WINDOWS

		if (::bind(nSock, (sockaddr*) &sAddrMy, sizeof(sockaddr_in)))
		{
			LOGERROR("Error : bind failed : %d", GetLastError());
			OnError(GetLastError());
			closesocket(nSock);
			m_fd = 0;
			return FALSE;
		}

		return TRUE;
	}

	size_t ITCPBASE::Recv(char* buf, int nMaxLen)
	{
		if (m_pbufferevent == NULL)
		{
			return 0;
		}
		if (buf == NULL)
		{
			return 0;
		}

		return bufferevent_read(m_pbufferevent, buf, nMaxLen);
	}
}