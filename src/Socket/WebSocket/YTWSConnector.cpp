#include "stdafx.h"
#include "YTWSConnector.h"

namespace YTSvrLib
{
	IWSCONNECTOR::IWSCONNECTOR()
	{
		m_sendBuf.Clear();
	}

	IWSCONNECTOR::~IWSCONNECTOR()
	{
		m_server = NULL;
		m_sendBuf.Clear();
	}

	void IWSCONNECTOR::Clean()
	{
		m_server = NULL;
		m_sendBuf.Clear();
	}

	void IWSCONNECTOR::Create(IWSSERVER* server, IWSSERVER::connection_ptr& con)
	{
		m_con = con;
		m_server = server;
	}

	void IWSCONNECTOR::Close()
	{
		LOG("IWSCONNECTOR : Close");
		try {
			if (m_server)
			{
				m_server->close(m_con->get_handle(), 0, "normal");
			}
		}
		catch (websocketpp::exception& e) {
			LOG("IWSCONNECTOR : Close Exception : code=%d what=%s",e.code().value(),e.what());
		}
	}

	void IWSCONNECTOR::Send(const char* msg, int len)
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
				if (m_sendBuf.AddBlock(msg, len))
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
				if (m_sendBuf.AddBlock(msg, len) == FALSE)
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
			LOG("Send AddBlock Error");
			Close();
		}
	}

	const websocketpp::connection_hdl IWSCONNECTOR::GetSocket() const
	{
		return m_con->get_handle();
	}

	std::string IWSCONNECTOR::GetIP() const
	{
		return m_con->get_remote_endpoint();
	}

	int IWSCONNECTOR::GetPort() const
	{
		return m_con->get_port();
	}

	bool IWSCONNECTOR::IsEnable()
	{
		if (m_con)
		{
			return true;
		}

		return false;
	}

	void IWSCONNECTOR::OnSend()
	{
		if (!IsEnable())
		{
			return;
		}

		const char* buf = m_sendBuf.GetDataToSend();
		int nLen = m_sendBuf.GetDataLenToSend();
		int nSend = 0;

		while (buf)
		{
			try
			{
				m_con->send((const void*)buf, (size_t)nLen);

				m_sendLock.Lock();
				BOOL bHasDataToSend = m_sendBuf.OnSend();
				m_sendLock.UnLock();

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
			catch (system_error const& e)
			{
				LOG("IWSCONNECTOR Send Exception : %d [%s]",e.code().value(),e.what());
				DWORD dwCode = GetLastError();

#ifdef LIB_WINDOWS
				if (dwCode != WSAEWOULDBLOCK)
#else
				if (dwCode != ENOMEM && dwCode != EAGAIN)
#endif // LIB_WINDOWS
				{
					LOG("Send Data Error : %d.Error : %d", nSend, dwCode);
					m_server->postWSEvent(this, WSEType_ClientClose);
					break;
				}
			}
		}
	}
}