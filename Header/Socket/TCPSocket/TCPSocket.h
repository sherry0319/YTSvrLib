#pragma once

#ifndef __YT_ASIO_SERVER_H_
#define __YT_ASIO_SERVER_H_

#include "../YTSocketThread.h"
#include "../SendBuffer.h"
#include "../MessageQueue.hpp"

namespace YTSvrLib {
	enum EM_ENDPOINT_TYPE {
		EP_NULL,
		EP_CONNECTOR,// 服务端的连接者
		EP_CLIENT,// 客户端(主动连接方)
	};

	class ITCPCONTROL;
	/**
	* 调用者基类.负责最终的socket调用,发送/读取
	*/
	class YTSVRLIB_EXPORT ITCPBASE {
	public:
		ITCPBASE();
	public:
		void Send(const char* buf, int len);
		virtual void SafeClose();
		std::string& GetAddrIp();
		int GetAddrPort();
		HANDLE GetSocket();
		BOOL IsSending() const;
		virtual EM_ENDPOINT_TYPE GetEPType() = 0;
	protected:
		void Clean();
		void DoRead();
		void DoSend();
		virtual int OnRecved(const char* msg, int nLen) = 0;
	protected:
		virtual void OnError(int nErrCode);
		void Close();
	protected:
		ITCPCONTROL* m_pSvrMgr;
		std::shared_ptr<asio::ip::tcp::socket> m_Socket;
		std::string m_strDstIP;
		int m_nPort;
		CBuffer<BLOCK_RECV_BUFFER_SIZE> m_recvBuf;
		CSendBuffer m_sendBuf;
		YTSvrLib::CLock m_sendLock;
		BOOL m_bIsSending;
	};

	class ITCPSERVER;
	/*
	* 服务器Accept后的连接者基类
	*/
	class YTSVRLIB_EXPORT ITCPCONNECTOR : public ITCPBASE {
	public:
		ITCPCONNECTOR() :ITCPBASE() {}
	public:
		void InitSvr(ITCPSERVER* pMgr, asio::ip::tcp::socket _sock);

		ITCPSERVER* GetMgr() {
			return (ITCPSERVER*)m_pSvrMgr;
		}

		virtual EM_ENDPOINT_TYPE GetEPType() {
			return EP_CONNECTOR;
		}
	};

	class ITCPCLIENTMGR;
	/* 客户端基类
	*/
	class YTSVRLIB_EXPORT ITCPCLIENT : public ITCPBASE {
	public:
		ITCPCLIENT();

		void Connect(ITCPCLIENTMGR* pMgr, const char* ip, int port);

		virtual void SafeClose() override;

		virtual void OnError(int nErrCode) override;

		void OnConnecting(std::error_code ec);

		virtual EM_ENDPOINT_TYPE GetEPType() {
			return EP_CLIENT;
		}

		ITCPCLIENTMGR* GetMgr() {
			return (ITCPCLIENTMGR*)m_pSvrMgr;
		}

		BOOL IsConnecting() const {
			return m_bIsConnecting;
		}

		BOOL IsConnected() const {
			return m_bConnected;
		}
	private:
		CLock m_lockStatus;
		BOOL m_bIsConnecting;
		BOOL m_bConnected;
	};

	/* 管理者基类.统一调度线程和消息队列 */
	class YTSVRLIB_EXPORT ITCPCONTROL : public IASIOTHREAD, public CMessageQueue<ITCPBASE> {};

	/* 服务器管理类 */
	class YTSVRLIB_EXPORT ITCPSERVER : public ITCPCONTROL {
	public:
		ITCPSERVER();

		virtual ITCPCONNECTOR* AllocateConnector(std::string dstIP) = 0;

		virtual void ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pConnect) = 0;

		BOOL StartListen(int nPort);
		
		void StopListen();
	private:
		virtual void ProcessDisconnectEvent(ITCPBASE* pConn) override;
	protected:
		void onAccept();
	protected:
		int m_nPort;
		asio::ip::tcp::acceptor m_Acceptor;
	};

	/* 客户端管理类.统一调度所有客户端在一个io_service之下 */
	class YTSVRLIB_EXPORT ITCPCLIENTMGR : public ITCPCONTROL {
	public:
		ITCPCLIENTMGR();

		void CreateThread();
	private:
		virtual void ProcessDisconnectEvent(ITCPBASE* pConn) override;
	};
}

#endif