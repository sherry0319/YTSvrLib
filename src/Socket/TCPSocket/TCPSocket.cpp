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
		char* buf = m_recvBuf.GetBuffer() + m_recvBuf.GetLength();
		int nMaxLen = (int)(m_recvBuf.GetCapacity() - m_recvBuf.GetLength());

		m_Socket->async_read_some(asio::buffer(buf, nMaxLen), [this](std::error_code ec, size_t readLength) {
			if (!ec)
			{
				if (readLength > 0)
				{
					m_recvBuf.AddBuffer(readLength);

					size_t nRead = OnRecved(m_recvBuf.GetBuffer(), (int)m_recvBuf.GetLength());

					if (nRead == 0 && m_recvBuf.GetLength() >= (int)m_recvBuf.GetCapacity()
						&& m_recvBuf.GetCapacity() < m_recvBuf.GetBufSizeMax())
					{
						size_t nNewBufSize = m_recvBuf.GetCapacity() + BLOCK_RECV_BUFFER_SIZE;
						if (nNewBufSize > m_recvBuf.GetBufSizeMax())
							nNewBufSize = m_recvBuf.GetBufSizeMax();
						m_recvBuf.ReSize(nNewBufSize);
					}

					m_recvBuf.ReleaseBuffer(nRead);
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
		if (len <= 0 || !buf || !m_Socket->is_open())
			return;

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
				}
				catch (std::exception& ec) {
					LOG("Close Socket Exception : [%s]", ec.what());
				}

				m_Socket->close();
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
		m_bConnected = FALSE;
		m_bIsConnecting = FALSE;
		if (m_Socket)
		{
			if (m_Socket->is_open())
			{
				try {
					m_Socket->shutdown(asio::socket_base::shutdown_both);
				}
				catch (std::exception& ec) {
					LOG("Close Socket Exception : [%s]", ec.what());
				}

				m_Socket->close();
			}
			m_Socket = NULL;
		}
	}

	void ITCPCLIENT::OnError(int nErrCode) {
		m_bConnected = FALSE;
		m_bIsConnecting = FALSE;
		ITCPBASE::OnError(nErrCode);
		GetMgr()->RemoveClient(this);
	}

	void ITCPCLIENT::Connect(ITCPCLIENTMGR* pMgr, const char* ip, int port) {
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

		m_bIsConnecting = TRUE;
		
		m_Socket->async_connect(_ep, [this, pMgr](std::error_code ec) {
			OnConnecting(ec);

			m_bIsConnecting = FALSE;

			if (!ec) {
				DoRead();
			} else {
				pMgr->RemoveClient(this);
			}
		});
		pMgr->AddClient(this);// 要先调用async_connect再启动线程循环,否则io_context会一直死循环导致cpu高占用
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
		m_setClients.clear();
	}

	void ITCPCLIENTMGR::CreateThread() {
		IASIOTHREAD::CreateThread("ITCPCLIENTMGR");
	}

	int ITCPCLIENTMGR::GetClientCount() const {
		return (int)m_setClients.size();
	}

	void ITCPCLIENTMGR::AddClient(ITCPCLIENT* pConn) {
		m_lockClientCount.Lock();
		m_setClients.insert(pConn);
		if (GetClientCount() > 0 && !IsRuning())
		{
			CreateThread();
		}
		m_lockClientCount.UnLock();
	}

	void ITCPCLIENTMGR::RemoveClient(ITCPCLIENT* pConn) {
		m_lockClientCount.Lock();
		m_setClients.erase(pConn);
		if (GetClientCount() <= 0 && IsRuning())
		{
			StopThread();
		}
		m_lockClientCount.UnLock();
	}
}