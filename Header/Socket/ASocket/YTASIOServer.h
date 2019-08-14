#pragma once

#ifndef __YT_ASIO_SERVER_H_
#define __YT_ASIO_SERVER_H_

#include "../YTSocketThread.h"

namespace YTSvrLib {
	class YTASIOSERVER;
	class YTSVRLIB_EXPORT YTASIOCONNECTOR {
	public:
		void Create(YTASIOSERVER* pMgr,asio::ip::tcp::socket _sock);
	protected:
		void DoRead();

		virtual int OnRecved(const char* msg, int nLen) = 0;
	protected:
		asio::ip::tcp::socket m_Socket;
		YTASIOSERVER* m_pSvrMgr;
		CBuffer<BLOCK_RECV_BUFFER_SIZE> m_recvBuf;
		CSendBuffer m_sendBuf;
	};

	class YTSVRLIB_EXPORT YTASIOSERVER : public IASIOTHREAD {
		YTASIOSERVER();

		typedef std::function<YTASIOCONNECTOR*(std::string)> AcceptorFunc;

		BOOL StartListen(int nPort, AcceptorFunc func);
	protected:
		void onAccept();
	protected:
		int m_nPort;
		asio::ip::tcp::acceptor m_Acceptor;
		AcceptorFunc m_funcAcceptor;
	};
}

#endif