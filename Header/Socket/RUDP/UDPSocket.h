#ifndef __YT_RUDP_SERVER_H_
#define __YT_RUDP_SERVER_H_

#include "../YTSocketThread.h"
#include "../SendBuffer.h"
#include "../MessageQueue.hpp"
#include "ikcp.h"

namespace YTSvrLib {

#define UDP_FLAG 0xEEFFEEFF

	struct UDPHEAD
	{
		UINT m_nFlag;
		UINT m_nConvId;// kcp会话id
	};

	class IUDPCONTROL;
	class YTSVRLIB_EXPORT IUDPBASE {
	public:
		IUDPBASE();

		void Clean();

		static int onKCPOutput(const char *buf, int len, ikcpcb *kcp, void *user);

		void InitKcp(IUINT32 conv);

		void RecvUDPData(const char* buf, int len, asio::ip::udp::endpoint remote);

		void OnKcpUpdate(__time32_t curTick);

		__time32_t GetCurTick();

		virtual __time32_t GetTimeoutTime();

		bool IsExpired();

		void SafeClose();

		void Send(const char *buf, int len);

		std::string GetAddrIp();
		int GetAddrPort();

		virtual int OnRecved(const char* msg, int nLen) = 0;
	protected:
		virtual void __Send(const char *buf, int len) = 0;
	protected:
		ikcpcb* m_kcpCore;
		IUINT32 m_conv;
		asio::ip::udp::endpoint m_remote;
		IUDPCONTROL* m_udpController;
		__time32_t m_lastPackRecvTime;

		BOOL m_bClosed;
		CBuffer m_recvBuf;
	};

	class YTSVRLIB_EXPORT IUDPSOCKET
	{
	public:
		IUDPSOCKET();

	public:
		void InitSocket(asio::io_service& core,int bindPort = 0, const char* bindIP = NULL);

		void CloseSocket();

		void DoUDPRecv();

		void OnUDPRecved(const asio::error_code& error, size_t bytes_recvd);

		void SendUDPData(const char* buf, int len, asio::ip::udp::endpoint& remote);

		virtual bool IsConnectPack(size_t bytes_recvd);

		virtual void HandleConnectPack(size_t bytes_recvd) = 0;

		virtual void HandleDataPack(size_t bytes_recvd) = 0;

		asio::ip::udp::endpoint& GetLastRemoteEndpoint() {
			return m_udpRemote;
		}

		const char* GetUDPBuffer() {
			return m_udpBuffer;
		}
	private:
		std::shared_ptr<asio::ip::udp::socket> m_udpSocket;

		char m_udpBuffer[1024 X256];

		asio::ip::udp::endpoint m_udpRemote;
	};

	/* 管理者基类.统一调度线程和消息队列 */
	class YTSVRLIB_EXPORT IUDPCONTROL : public IASIOTHREAD, public CMessageQueue<IUDPBASE> {
	public:
		IUDPCONTROL();
	public:
		void InitKcpTimer();

		__time32_t GetCurTick() const {
			return m_curTick;
		}

		void StartKcpTimer();

		void OnKcpTimer();

		virtual void HandleKcpTimer() = 0;
	private:
		__time32_t m_curTick;
		std::shared_ptr<asio::steady_timer> m_kcpTimer;
	};

	class IUDPSERVER;
	class YTSVRLIB_EXPORT IUDPCONNECTOR : public IUDPBASE
	{
	public:
		virtual void Init(IUDPSERVER* udpSvr, IUINT32 conv, asio::ip::udp::endpoint& remote);
	protected:
		virtual void __Send(const char *buf, int len) override;
	};

	class YTSVRLIB_EXPORT IUDPSERVER : public IUDPSOCKET, public IUDPCONTROL {
	public:
		IUDPSERVER();
	public:
		BOOL StartListen(int nPort);

		void StopListen();

		IUINT32 GetNewConvID() const;
	
		virtual IUDPCONNECTOR* AllocateConnector(std::string dstIP) = 0;

		virtual void ReleaseConnector(IUDPCONNECTOR* pConn) = 0;
	
		virtual void HandleConnectPack(size_t bytes_recvd) override;

		virtual void HandleDataPack(size_t bytes_recvd) override;

		virtual void HandleKcpTimer() override;

		virtual void ProcessDisconnectEvent(IUDPBASE* pConn) override;
	private:
		int m_nPort;

		std::unordered_map<IUINT32, IUDPBASE*> m_mapUDPConnections;
	};

	class IUDPCLIENTMGR;
	class YTSVRLIB_EXPORT IUDPCLIENT : public IUDPSOCKET, public IUDPBASE
	{
	public:
		IUDPCLIENT();

		void Connect(IUDPCLIENTMGR* pMgr, const char* destIP, int destPort);

		bool IsConnected();
	public:
		virtual void HandleConnectPack(size_t bytes_recvd) override;

		virtual void HandleDataPack(size_t bytes_recvd) override;
	protected:
		virtual void __Send(const char *buf, int len) override;// 这不是给外面用的发送函数，是经过kcp处理后的发送
	protected:
		BOOL m_bConnecting;
	};

	class YTSVRLIB_EXPORT IUDPCLIENTMGR : public IUDPCONTROL {
	public:
		IUDPCLIENTMGR();

		void CreateThread();
	private:
		virtual void ProcessDisconnectEvent(IUDPBASE* pConn) override;
	};
}

#endif
