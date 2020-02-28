#include "stdafx.h"
#include "UDPSocket.h"

namespace YTSvrLib {
	/* get system time */
	static inline void itimeofday(long *sec, long *usec)
	{
#ifndef LIB_WINDOWS
		struct timeval time;
		gettimeofday(&time, NULL);
		if (sec) *sec = time.tv_sec;
		if (usec) *usec = time.tv_usec;
#else
		static long mode = 0, addsec = 0;
		BOOL retval;
		static IINT64 freq = 1;
		IINT64 qpc;
		if (mode == 0) {
			retval = QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
			freq = (freq == 0) ? 1 : freq;
			retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
			addsec = (long)time(NULL);
			addsec = addsec - (long)((qpc / freq) & 0x7fffffff);
			mode = 1;
		}
		retval = QueryPerformanceCounter((LARGE_INTEGER*)&qpc);
		retval = retval * 2;
		if (sec) *sec = (long)(qpc / freq) + addsec;
		if (usec) *usec = (long)((qpc % freq) * 1000000 / freq);
#endif
	}

	/* get clock in millisecond 64 */
	static inline IINT64 iclock64(void)
	{
		long s, u;
		IINT64 value;
		itimeofday(&s, &u);
		value = ((IINT64)s) * 1000 + (u / 1000);
		return value;
	}

	static inline IUINT32 iclock()
	{
		return (IUINT32)(iclock64() & 0xfffffffful);
	}

	IUDPBASE::IUDPBASE()
	{
		Clean();
	}

	void IUDPBASE::Clean()
	{
		m_conv = 0;
		m_kcpCore = NULL;
		m_udpController = NULL;
		m_lastPackRecvTime = 0;
		m_bClosed = FALSE;
	}

	void IUDPBASE::InitKcp(IUINT32 conv)
	{
		m_kcpCore = ikcp_create(conv, (void*)this);
		m_kcpCore->output = &IUDPBASE::onKCPOutput;
		// 启动快速模式
		// 第二个参数 nodelay-启用以后若干常规加速将启动
		// 第三个参数 interval为内部处理时钟，默认设置为 10ms
		// 第四个参数 resend为快速重传指标，设置为2
		// 第五个参数 为是否禁用常规流控，这里禁止
		//ikcp_nodelay(p_kcp_, 1, 10, 2, 1);
		ikcp_nodelay(m_kcpCore, 1, 5, 1, 1); // 设置成1次ACK跨越直接重传, 这样反应速度会更快. 内部时钟5毫秒.

	}

	int IUDPBASE::onKCPOutput(const char *buf, int len, ikcpcb *, void *user)
	{
		((IUDPBASE*)user)->__Send(buf, len);
		return 0;
	}

	void IUDPBASE::RecvUDPData(const char* buf, int len, asio::ip::udp::endpoint remote)
	{
		m_remote = remote;

		m_lastPackRecvTime = GetCurTick();

		ikcp_input(m_kcpCore, buf, len);

		{
			char kcp_buf[1024 X512] = "";
			int kcp_recvd_bytes = ikcp_recv(m_kcpCore, kcp_buf, sizeof(kcp_buf));
			if (kcp_recvd_bytes > 0)
			{
				m_recvBuf.AddBuffer(kcp_buf, kcp_recvd_bytes);

				size_t nRead = OnRecved(m_recvBuf.GetBuffer(), (int)m_recvBuf.GetLength());// 上层逻辑处理

				if (nRead > 0)
				{
					m_recvBuf.ReleaseBuffer(nRead);
				}
			}
		}
	}

	void IUDPBASE::Send(const char *buf, int len)
	{
		if (m_kcpCore)
		{
			int ret = ikcp_send(m_kcpCore, buf, len);
			if (ret < 0)
			{
				LOG("Send kcp data error : %d",ret);
			}
		}
	}

	void IUDPBASE::OnKcpUpdate(__time32_t curTick)
	{
		ikcp_update(m_kcpCore, curTick);
	}

	__time32_t IUDPBASE::GetCurTick()
	{
		if (m_udpController == NULL)
		{
			return 0;
		}

		return m_udpController->GetCurTick();
	}

	__time32_t IUDPBASE::GetTimeoutTime()
	{
		return 10 * 1000;
	}

	bool IUDPBASE::IsExpired()
	{
		if (m_lastPackRecvTime == 0)
		{
			return false;
		}

		return (GetCurTick() - m_lastPackRecvTime) > GetTimeoutTime();
	}

	void IUDPBASE::SafeClose() {
		if (m_bClosed)
		{
			return;
		}
		m_bClosed = TRUE;
		if (m_udpController)
		{
			m_udpController->AddNewMessage(MSGTYPE_DISCONNECT, this);
		}
	}

	std::string IUDPBASE::GetAddrIp()
	{
		return m_remote.address().to_string();
	}

	int IUDPBASE::GetAddrPort()
	{
		return (int)m_remote.port();
	}

	//////////////////////////////////////////////////////////////////////////

	IUDPSOCKET::IUDPSOCKET()
	{
		m_udpSocket = NULL;
		ZeroMemory(m_udpBuffer, sizeof(m_udpBuffer));
	}

	void IUDPSOCKET::InitSocket(asio::io_service& core, int bindPort, const char* bindIP)
	{
		if (!m_udpSocket)
		{
			asio::ip::udp::endpoint ep;
			if (bindIP != NULL && strlen(bindIP) >= 3/*最短的IPv6回环*/)
			{
				ep = asio::ip::udp::endpoint(asio::ip::address::from_string(bindIP), (unsigned short)bindPort);
			}
			else
			{
				ep = asio::ip::udp::endpoint(asio::ip::udp::v4(), (unsigned short)bindPort);
			}

			m_udpSocket = std::make_shared<asio::ip::udp::socket>(core, ep);
		}
	}

	void IUDPSOCKET::DoUDPRecv()
	{
		if (m_udpSocket && m_udpSocket->is_open())
		{
			ZeroMemory(m_udpBuffer, sizeof(m_udpBuffer));
			m_udpSocket->async_receive_from(
				asio::buffer(m_udpBuffer, sizeof(m_udpBuffer)), m_udpRemote,
				std::bind(&IUDPSOCKET::OnUDPRecved, this,
					std::placeholders::_1, std::placeholders::_2));
		}
	}

	void IUDPSOCKET::CloseSocket()
	{
		try
		{
			if (m_udpSocket && m_udpSocket->is_open())
			{
				m_udpSocket->shutdown(asio::socket_base::shutdown_both);

				m_udpSocket->cancel();

				m_udpSocket->close();

				m_udpSocket = NULL;
			}
		}
		catch (std::exception& ec)
		{
			LOG("TCP Server Start Listen Error : [%s]", ec.what());
		}
	}

	void IUDPSOCKET::OnUDPRecved(const asio::error_code& error, size_t bytes_recvd)
	{
		if (!error && bytes_recvd > 0)
		{
			if (IsConnectPack(bytes_recvd))
			{
				HandleConnectPack(bytes_recvd);
			}
			else
			{
				HandleDataPack(bytes_recvd);
			}
		}
		else
		{
			LOG("onUDPRecved error end! error: %s, bytes_recvd: %ld\n", error.message().c_str(), (long)bytes_recvd);
		}

		DoUDPRecv();
	}

	void IUDPSOCKET::SendUDPData(const char* buf, int len, asio::ip::udp::endpoint& remote)
	{
		if (m_udpSocket && m_udpSocket->is_open())
		{
			m_udpSocket->send_to(asio::buffer(buf, len), remote);
		}
	}

	bool IUDPSOCKET::IsConnectPack(size_t bytes_recvd)
	{
		static const size_t UDP_HEAD_SIZE = sizeof(UDPHEAD);
		if (bytes_recvd >= UDP_HEAD_SIZE)
		{
			UDPHEAD* pHead = (UDPHEAD*)m_udpBuffer;

			if (pHead->m_nFlag == UDP_FLAG)
			{
				return true;
			}
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////

	IUDPCONTROL::IUDPCONTROL()
	{
		m_curTick = 0;
		m_kcpTimer = NULL;
	}

	void IUDPCONTROL::InitKcpTimer() {
		if (!m_kcpTimer)
		{
			m_kcpTimer = std::make_shared<asio::steady_timer>(GetCore());
		}
	}

	void IUDPCONTROL::StartKcpTimer()
	{
		if (m_kcpTimer && IsRuning())
		{
			m_kcpTimer->expires_after(std::chrono::milliseconds(5));
			m_kcpTimer->async_wait(std::bind(&IUDPCONTROL::OnKcpTimer, this));
		}
	}

	void IUDPCONTROL::OnKcpTimer()
	{
		StartKcpTimer();
		m_curTick = (__time32_t)iclock();

		HandleKcpTimer();
	}

	//////////////////////////////////////////////////////////////////////////

	void IUDPCONNECTOR::Init(IUDPSERVER* udpSvr, IUINT32 conv, asio::ip::udp::endpoint& remote)
	{
		m_udpController = udpSvr;
		m_conv = conv;
		m_remote = remote;
		m_lastPackRecvTime = 0;
		m_bClosed = FALSE;

		InitKcp(conv);
	}

	void IUDPCONNECTOR::__Send(const char *buf, int len)
	{
		if (m_udpController)
		{
			IUDPSERVER* udpSvr = dynamic_cast<IUDPSERVER*>(m_udpController);
			if (udpSvr)
			{
				udpSvr->SendUDPData(buf, len, m_remote);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////

	IUDPSERVER::IUDPSERVER() {
		m_nPort = 0;
		m_mapUDPConnections.clear();
	}

	IUINT32 IUDPSERVER::GetNewConvID() const
	{
		IUINT32 newID = Random2(INT_MAX, 1000);

		while (m_mapUDPConnections.find(newID) != m_mapUDPConnections.end())
		{
			newID = Random2(INT_MAX, 1000);
		}

		return newID;
	}

	BOOL IUDPSERVER::StartListen(int nPort) {
		m_nPort = nPort;

		InitSocket(GetCore(),nPort);
		InitKcpTimer();

		CreateThread("IUDPSERVER");

		DoUDPRecv();

		StartKcpTimer();

		return TRUE;
	}

	void IUDPSERVER::HandleConnectPack(size_t) {
		IUINT32 conv = GetNewConvID();

		std::string strIP = GetLastRemoteEndpoint().address().to_string();

		IUDPCONNECTOR* pConn = AllocateConnector(strIP);

		if (pConn)
		{
			char buffer[1024] = { 0 };
			UDPHEAD* pHead = (UDPHEAD*)buffer;
			pHead->m_nFlag = UDP_FLAG;
			pHead->m_nConvId = conv;

			SendUDPData(buffer, sizeof(UDPHEAD), GetLastRemoteEndpoint());

			pConn->Init(this, conv, GetLastRemoteEndpoint());

			m_mapUDPConnections[conv] = pConn;
		}
	}

	void IUDPSERVER::HandleDataPack(size_t bytes_recvd) {
		IUINT32 conv = ikcp_getconv(GetUDPBuffer());
		if (conv == 0)
		{
			LOG("ikcp_get_conv return 0");
			return;
		}

		if (m_mapUDPConnections.find(conv) != m_mapUDPConnections.end())
		{
			IUDPBASE* pConn = m_mapUDPConnections[conv];

			pConn->RecvUDPData(GetUDPBuffer(), (int)bytes_recvd, GetLastRemoteEndpoint());
		}
		else
		{
			LOG("can not find kcp conv id = %ld",conv);
		}
	}

	void IUDPSERVER::HandleKcpTimer()
	{
		for (auto it = m_mapUDPConnections.begin(); it != m_mapUDPConnections.end(); )
		{
			IUDPBASE* pConn = it->second;

			pConn->OnKcpUpdate(GetCurTick());

			// check timeout
			if (pConn->IsExpired())
			{
				pConn->SafeClose();
				it = m_mapUDPConnections.erase(it++);
				continue;
			}

			it++;
		}
	}

	void IUDPSERVER::StopListen() {
		m_mapUDPConnections.clear();

		CloseSocket();

		StopThread();
	}

	void IUDPSERVER::ProcessDisconnectEvent(IUDPBASE* pConn) {
		ProcessEvent(MSGTYPE_DISCONNECT, pConn);

		IUDPCONNECTOR* pUDPConn = dynamic_cast<IUDPCONNECTOR*>(pConn);
		if (pUDPConn)
		{
			ReleaseConnector(pUDPConn);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	
	IUDPCLIENTMGR::IUDPCLIENTMGR()
	{

	}

	void IUDPCLIENTMGR::CreateThread()
	{
		IASIOTHREAD::CreateThread("IUDPCLIENTMGR");
	}

	void IUDPCLIENTMGR::ProcessDisconnectEvent(IUDPBASE* pConn)
	{
		ProcessEvent(MSGTYPE_DISCONNECT, pConn);

		pConn->SafeClose();
	}

	//////////////////////////////////////////////////////////////////////////

	IUDPCLIENT::IUDPCLIENT()
	{
		m_bConnecting = FALSE;
	}

	void IUDPCLIENT::Connect(IUDPCLIENTMGR* pMgr, const char* destIP, int destPort)
	{
		if (m_bConnecting)
		{
			LOG("Connecting to udp server");
			return;
		}

		m_bConnecting = TRUE;
		m_udpController = pMgr;
		m_conv = 0;
		m_remote = asio::ip::udp::endpoint(asio::ip::address::from_string(destIP),(unsigned short)destPort);
		m_lastPackRecvTime = 0;
		m_bClosed = FALSE;

		char buffer[64] = { 0 };
		UDPHEAD* pHead = (UDPHEAD*)buffer;
		pHead->m_nFlag = UDP_FLAG;
		pHead->m_nConvId = 0;

		InitSocket(pMgr->GetCore());

		SendUDPData(buffer, sizeof(UDPHEAD), m_remote);
	}

	bool IUDPCLIENT::IsConnected()
	{
		return (m_conv > 0);
	}

	void IUDPCLIENT::HandleConnectPack(size_t bytes_recvd)
	{
		if (bytes_recvd >= sizeof(UDPHEAD))
		{
			UDPHEAD* pHead = (UDPHEAD*)GetUDPBuffer();
			if (pHead && pHead->m_nFlag == UDP_FLAG && pHead->m_nConvId > 0)
			{
				m_conv = pHead->m_nConvId;

				InitKcp(m_conv);

				m_udpController->AddNewMessage(MSGTYPE_CONNECTED, this);
			}
			else
			{
				m_conv = 0;
				m_bConnecting = FALSE;

				m_udpController->AddNewMessage(MSGTYPE_CONNECTFAILED, this);
			}
		}
		else 
		{
			m_conv = 0;
			m_bConnecting = FALSE;
			m_udpController->AddNewMessage(MSGTYPE_CONNECTFAILED, this);
		}
	}

	void IUDPCLIENT::HandleDataPack(size_t bytes_recvd)
	{
		IUINT32 conv = ikcp_getconv(GetUDPBuffer());
		if (conv != m_conv)
		{
			LOG("ikcp_get_conv return 0");
			return;
		}

		RecvUDPData(GetUDPBuffer(), (int)bytes_recvd, GetLastRemoteEndpoint());
	}

	void IUDPCLIENT::__Send(const char *buf, int len)
	{
		if (m_bConnecting || !IsConnected())
		{
			return;
		}
		SendUDPData(buf, len, m_remote);
	}
}