#include "stdafx.h"
#include "YTWSConnector.h"

namespace YTSvrLib
{
	IWSCONNECTOR::IWSCONNECTOR()
	{
		m_ctx = NULL;
		m_session = NULL;
		m_sendBuf.Clear();
	}

	IWSCONNECTOR::~IWSCONNECTOR()
	{
		m_ctx = NULL;
		m_session = NULL;
		m_server = NULL;
		m_sendBuf.Clear();
	}

	void IWSCONNECTOR::Clean()
	{
		m_ctx = NULL;
		m_session = NULL;
		m_server = NULL;
		m_sendBuf.Clear();
	}

	void IWSCONNECTOR::Create(IWSSERVER* server, lws* ctx, IWSSESSION* session)
	{
		m_ctx = ctx;
		m_session = session;
		m_server = server;
	}

	void IWSCONNECTOR::Close()
	{
		LOG("IWSCONNECTOR : Close");
		if (m_server && m_ctx)
		{
			m_server->SetContextEnable(m_ctx, this, false);
		}
		IWSSERVER::WaitWritable(m_ctx);
	}

	void IWSCONNECTOR::WaitWritable()
	{
		IWSSERVER::WaitWritable(m_ctx);
	}

	void IWSCONNECTOR::Send(const char* msg, int len, lws_write_protocol type /*= LWS_WRITE_TEXT*/)
	{
		if (len <= 0 || !msg || !IsEnable())
			return;

		BOOL bSend = FALSE;
		BOOL bError = FALSE; //是否有错误发生

		m_sendLock.Lock();
		try
		{
			if (m_sendBuf.IsSending() == FALSE)
			{
				if (m_sendBuf.AddBlock(msg, len,type))
				{
					bSend = TRUE;
				}
				else
				{
					bError = TRUE;
				}
			}
			else
			{
				if (m_sendBuf.AddBlock(msg, len, type) == FALSE)
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
			WaitWritable();
		}

		if (bError)
		{
			LOG("Send AddBlock Error");
			Close();
		}
	}

	SOCKET IWSCONNECTOR::GetSocket()
	{
		if (m_session == NULL)
		{
			return (SOCKET)0;
		}
		return m_session->socket;
	}

	const char* IWSCONNECTOR::GetIP()
	{
		if (m_session == NULL)
		{
#ifdef DEBUG64
			return NULL;
#else
			return "";
#endif
		}
		return m_session->ip;
	}

	int IWSCONNECTOR::GetPort()
	{
		if (m_session == NULL)
		{
			return 0;
		}
		return m_session->port;
	}

	bool IWSCONNECTOR::IsEnable()
	{
		if (m_ctx == NULL || m_server == NULL || m_session == NULL)
			return false;
		if (m_server->GetConnector(m_ctx) == NULL)
			return false;

		return true;
	}

	void IWSCONNECTOR::OnSend()
	{
		if (!IsEnable())
		{
			return;
		}

		const char* buf = m_sendBuf.GetDataToSend();
		int nLen = m_sendBuf.GetDataLenToSend();
		lws_write_protocol type = m_sendBuf.GetDataTypeToSend();
		int nSend = 0;

		if (buf)
		{
			SetSysLastError(0);
			if ((nSend = IWSSERVER::Send(m_ctx, buf, nLen, type)) >= 0)
			{
				// LOG("Data Send = %d/%d",nSend,nLen);
				m_sendLock.Lock();
				BOOL bHasDataToSend = m_sendBuf.OnSend();
				m_sendLock.UnLock();

				if (!IsEnable())
				{
					return;
				}

				if (bHasDataToSend)
				{
					WaitWritable();
				}

				return;
			}
			else
			{
				DWORD dwCode = GetLastError();

				if (nSend < 0 && dwCode == 0)
				{// 处理特殊情况.websocket临时断开处理写请求
					WaitWritable();
					return;
				}

#ifdef LIB_WINDOWS
				if (dwCode != WSAEWOULDBLOCK)
#else
				if (dwCode != ENOMEM && dwCode != EAGAIN)
#endif // LIB_WINDOWS
				{
					LOG("Send Data Error : %d.Error : %d", nSend, dwCode);
					Close();
					return;
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

					int ret = select((int) GetSocket() + 1, NULL, &fds, NULL, &timeout);
					if (ret <= 0)
					{
						LOG("Send Data Error Select : %d.Error : %d", nSend, dwCode);
						Close();
						return;
					}
				}
			}
		}
	}
}