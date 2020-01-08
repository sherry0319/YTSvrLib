#include "stdafx.h"
#include "TCPSocket.h"
#include <asio/ip/tcp.hpp>

namespace YTSvrLib {
	ITCPBASE::ITCPBASE() {
		Clean();
	}

	std::string& ITCPBASE::GetAddrIp() {
		return m_strDstIP;
	}

	int ITCPBASE::GetAddrPort() {
		return m_nPort;
	}

	HANDLE ITCPBASE::GetSocket() {
		if (!m_Socket || !m_Socket->is_open())
		{
			return INVALID_HANDLE_VALUE;
		}

		return (HANDLE)(size_t)m_Socket->native_handle();
	}

	void ITCPBASE::Clean() {
		m_pSvrMgr = NULL;
		m_bIsSending = FALSE;
		m_recvBuf.Clear();
		m_sendBuf.Clear();
		m_strDstIP.clear();
		m_nPort = 0;
		Close();
	}

	void ITCPBASE::DoRead() {
		std::string& buf = m_recvBuf.GetRecvingBuffer();

		m_Socket->async_read_some(asio::buffer(buf), [this,&buf](std::error_code ec, size_t readLength) {
			if (!ec)
			{
				if (readLength > 0)
				{
					m_recvBuf.AddBuffer(buf.c_str(),readLength);

					size_t nRead = OnRecved(m_recvBuf.GetBuffer(), (int)m_recvBuf.GetLength());

					if (nRead > 0)
					{
						m_recvBuf.ReleaseBuffer(nRead);
					}
				}

				DoRead();
			}
			else {
				OnError(ec.value());
			}
		});
	}

	BOOL ITCPBASE::IsSending() const {
		return m_bIsSending;
	}

	void ITCPBASE::DoSend() {
		if (!m_Socket || !m_Socket->is_open()) {
			return;
		}
		char* buf = m_sendBuf.GetDataToSend();
		int nLen = m_sendBuf.GetDataLenToSend();
		m_bIsSending = TRUE;

		asio::async_write(*m_Socket, asio::buffer(buf, nLen), [this](std::error_code ec, size_t writeLength) {
			if (!ec)
			{// 正确发送
				m_sendLock.Lock();
				BOOL bHasDataToSend = m_sendBuf.OnSend((int)writeLength);
				m_sendLock.UnLock();

				if (bHasDataToSend) {
					DoSend();
				} else {
					m_bIsSending = FALSE;
				}
			}
			else {
				OnError(ec.value());
			}
		});
	}

	void ITCPBASE::Send(const char* buf,int len) {
		if (len <= 0 || !buf)
			return;
		if (!m_Socket || !m_Socket->is_open()) {
			return;
		}

		BOOL bSend = TRUE;
		BOOL bSucc = FALSE;
		m_sendLock.Lock();
		try
		{
			if (m_sendBuf.IsSending())
			{
				bSend = FALSE;
			}
			bSucc = m_sendBuf.AddBuffer(buf, len);
		}
		catch (...)
		{
			m_sendLock.UnLock();
			return;
		}

		m_sendLock.UnLock();

		if (!bSucc)
		{
			OnError(0);
		}
		if (bSend)
		{
			DoSend();
		}
	}

	void ITCPBASE::Close() {
		if (m_Socket)
		{
			if (m_Socket->is_open())
			{
				try {
					m_Socket->shutdown(asio::socket_base::shutdown_both);
					if (m_Socket)
					{
						m_Socket->close();
					}
				}
				catch (std::exception& ec) {
					LOG("Close Socket Exception : [%s]", ec.what());
				}
			}
			m_Socket = NULL;
		}
	}

	void ITCPBASE::OnError(int nErrCode) {
		LOG("Connector Error : %d", nErrCode);
		Close();
		if (m_pSvrMgr)
		{
			m_pSvrMgr->AddNewMessage(MSGTYPE_DISCONNECT, this);
		}
	}

	void ITCPBASE::SafeClose() {
		if (m_Socket)
		{
			Close();
		}
	}

	//////////////////////////////////////////////////////////////////////////

	void ITCPCONNECTOR::InitSvr(ITCPSERVER* pMgr, asio::ip::tcp::socket _sock) {
		m_Socket = std::shared_ptr<asio::ip::tcp::socket>(new asio::ip::tcp::socket(std::move(_sock)));
		m_pSvrMgr = pMgr;
		auto _ep = m_Socket->remote_endpoint();
		m_strDstIP = _ep.address().to_string();
		m_nPort = (int)_ep.port();
		m_recvBuf.Clear();
		m_sendBuf.Clear();
		DoRead();
	}

	ITCPCLIENT::ITCPCLIENT():ITCPBASE() {
		m_recvBuf.Clear();
		m_sendBuf.Clear();
		m_bConnected = FALSE;
		m_bIsConnecting = FALSE;
	}

	void ITCPCLIENT::SafeClose() {
		m_lockStatus.Lock();
		m_bConnected = FALSE;
		m_bIsConnecting = FALSE;
		m_lockStatus.UnLock();
		Close();
	}

	void ITCPCLIENT::OnError(int nErrCode) {
		m_lockStatus.Lock();
		m_bConnected = FALSE;
		m_bIsConnecting = FALSE;
		m_lockStatus.UnLock();
		ITCPBASE::OnError(nErrCode);
	}

	void ITCPCLIENT::Connect(ITCPCLIENTMGR* pMgr, const char* ip, int port) {
		if (IsConnecting())
		{// 连接中,禁止重复连接
			return;
		}
		m_pSvrMgr = pMgr;
		m_strDstIP = ip;
		m_nPort = port;
		asio::ip::tcp::endpoint _ep(asio::ip::address::from_string(ip), (unsigned short)port);

		if (!m_Socket)
		{
			m_Socket = std::make_shared<asio::ip::tcp::socket>(GetMgr()->GetCore());
		}
		if (!m_Socket->is_open())
		{
			m_Socket->open(_ep.protocol());
			m_Socket->set_option(asio::ip::tcp::no_delay(true));
		}

		m_lockStatus.Lock();
		m_bIsConnecting = TRUE;
		m_lockStatus.UnLock();

		m_Socket->async_connect(_ep, [this, pMgr](std::error_code ec) {
			m_lockStatus.Lock();
			OnConnecting(ec);
			m_bIsConnecting = FALSE;
			m_lockStatus.UnLock();

			if (!ec) {
				DoRead();
			}
		});
	}

	void ITCPCLIENT::OnConnecting(std::error_code ec) {
		if (!ec)
		{
			m_bConnected = TRUE;
			LOG("Connection Established : %x", this);
			GetMgr()->AddNewMessage(MSGTYPE_CONNECTED, this);
		}
		else
		{
			m_bConnected = FALSE;
			LOG("Connection Failed : %x ErrorCode=%d", this, ec.value());
			GetMgr()->AddNewMessage(MSGTYPE_CONNECTFAILED, this);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	ITCPSERVER::ITCPSERVER():m_Acceptor(GetCore()) {}

	void ITCPSERVER::onAccept() {
		m_Acceptor.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
			if (!ec)
			{
				ITCPCONNECTOR* pConn = NULL;
				try {
					std::string remote = socket.remote_endpoint().address().to_string();

					LOG("onAccept Remote IP = [%s]",remote.c_str());

					pConn = AllocateConnector(remote);

					if (pConn == NULL)
					{
						try {
							socket.shutdown(asio::socket_base::shutdown_both);
						} catch (std::exception& ec) {
							LOGTRACE("Close Connection Exception : [%s]",ec.what());
						}
						socket.close();
						return;
					}

					pConn->InitSvr(this, std::move(socket));

					AddNewMessage(MSGTYPE_ACCEPTED, pConn);

					pConn = NULL;
				}
				catch (std::exception& e) {
					if (pConn) {
						ProcessEvent(MSGTYPE_DISCONNECT, pConn);
						pConn = NULL;
					}
					LOG("onAccept Exception : [%s]",e.what());
				}
			}

			onAccept();
		});
	}

	void ITCPSERVER::StopListen() {
		if (m_Acceptor.is_open())
		{
			m_Acceptor.close();
		}

		StopThread();
	}

	BOOL ITCPSERVER::StartListen(int nPort) {
		m_nPort = nPort;
		auto ep = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), (unsigned short)nPort);
		if (!m_Acceptor.is_open())
		{
			m_Acceptor.open(ep.protocol());
#ifndef LIB_WINDOWS
			m_Acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
#endif // LIB_WINDOWS
		}

		try {
			m_Acceptor.bind(ep);
			m_Acceptor.listen();
		}
		catch (std::exception& ec) {
			LOG("TCP Server Start Listen Error : [%s]", ec.what());
			return FALSE;
		}

		onAccept();

		CreateThread("ITCPSERVER");

		return TRUE;
	}

	void ITCPSERVER::ProcessDisconnectEvent(ITCPBASE* pConn) {
		ProcessEvent(MSGTYPE_DISCONNECT, pConn);

		ITCPCONNECTOR* pConnector = dynamic_cast<ITCPCONNECTOR*>(pConn);

		if (pConnector)
		{
			ReleaseConnector(pConnector);
		}
	}

	//////////////////////////////////////////////////////////////////////////

	ITCPCLIENTMGR::ITCPCLIENTMGR() {
		CreateThread();
	}

	void ITCPCLIENTMGR::CreateThread() {
		IASIOTHREAD::CreateThread("ITCPCLIENTMGR");
	}

	void ITCPCLIENTMGR::ProcessDisconnectEvent(ITCPBASE* pConn) {
		ProcessEvent(MSGTYPE_DISCONNECT, pConn);

		pConn->SafeClose();
	}
}