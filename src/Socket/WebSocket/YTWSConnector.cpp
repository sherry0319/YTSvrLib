#include "stdafx.h"
#include "YTWSConnector.h"

namespace YTSvrLib
{
	IWSCONNECTOR::IWSCONNECTOR()
	{
		Clean();
	}

	IWSCONNECTOR::~IWSCONNECTOR()
	{
		Clean();
	}

	void IWSCONNECTOR::Clean()
	{
		m_server = NULL;
		m_sendBuf.Clear();
		m_strIP.clear();
		m_strIP.shrink_to_fit();
		m_nPort = 0;
	}

	void IWSCONNECTOR::Create(IWSSERVER* server, IWSSERVER::connection_ptr& con)
	{
		m_con = con;
		m_server = server;

		if (m_con)
		{
			std::string remote = con->get_remote_endpoint(); // [::ffff:127.0.0.1]:50331
			remote = remote.substr(1, remote.length() - 1); // 去掉开头的[
			std::vector<std::string> vctList;
			StrDelimiter(remote, "]", vctList);// 0=IP 1=:端口

			if (vctList[0].length() > 0)
			{// 处理IP
				std::vector<std::string> vctIP;
				StrDelimiter(vctList[0], ":", vctIP);

				m_strIP = vctIP[(vctIP.size() - 1)];
			}
			if (vctList[1].length() > 0)
			{// 处理端口
				std::string port = vctList[1].substr(1, vctList[1].length() - 1);// 去掉开头的:即可
				m_nPort = atoi(port.c_str());
			}
		}
	}

	void IWSCONNECTOR::SafeClose()
	{
		LOG("IWSCONNECTOR : SafeClose");
		try {
			if (m_server)
			{
				m_server->close(m_con->get_handle(), 0, "normal closed");

				m_server = NULL;
			}
		}
		catch (websocketpp::exception& e) {
			LOG("IWSCONNECTOR : SafeClose Exception : code=%d what=%s",e.code().value(),e.what());
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
			SafeClose();
		}
	}

	SOCKET_HANDLE IWSCONNECTOR::GetSocket() const
	{
		auto info = m_con->get_handle().lock();
		return (SOCKET_HANDLE)info.get();
	}

	std::string& IWSCONNECTOR::GetAddrIp()
	{
		return m_strIP;
	}

	int IWSCONNECTOR::GetAddrPort() const
	{
		return m_nPort;
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
					m_server->AddNewMessage(MSGTYPE_DISCONNECT, this);
					break;
				}
			}
		}
	}
}