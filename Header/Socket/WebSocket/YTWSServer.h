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

#pragma once

#include <websocketpp/common/asio.hpp>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <asio/ssl.hpp>
#include "WSSendBuffer.h"
#include "../MessageQueue.hpp"

typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

namespace YTSvrLib
{
	class IWSCONNECTOR;
	class YTSVRLIB_EXPORT IWSSERVER : public websocketpp::server<websocketpp::config::asio_tls>,public IASIOTHREAD, public CMessageQueue<IWSCONNECTOR>
	{
	public:
		IWSSERVER();
		virtual ~IWSSERVER();
	public:
		BOOL StartListen(int nPort, const char* ssl_cert_filepath = NULL, const char* ssl_private_key_filepath = NULL,BOOL bDebug = FALSE);

		void StopListen();
	public:
		// 注意,会从不同线程调入.加锁!
		virtual IWSCONNECTOR* AllocateConnector() = 0;
		virtual void ReleaseConnector(IWSCONNECTOR* pConn) = 0;
		virtual bool validateClient(std::string& dstIP) = 0;
		virtual IWSCONNECTOR* GetConnector(websocketpp::connection_hdl hdl);

		// 注意,会从不同线程调入.加锁!
		virtual void SetContextEnable(websocketpp::connection_hdl hdl,IWSCONNECTOR* connectior);
		virtual void SetContextDisable(websocketpp::connection_hdl hdl);
	private:
		void onWSConnected(websocketpp::connection_hdl hdl);

		void onWSClosed(websocketpp::connection_hdl hdl);

		void onWSDataRecv(websocketpp::connection_hdl hdl, message_ptr msg);

		context_ptr onWSTLSInit(websocketpp::connection_hdl hdl);

		bool onWSValidate(websocketpp::connection_hdl hdl);
	private:
		virtual void ProcessDisconnectEvent(IWSCONNECTOR* pConn) override;
	private:
		std::string _ssl_cert;
		std::string _ssl_key;
		int _port;

		YTSvrLib::CLock _connector_lock;
		typedef std::map<websocketpp::connection_hdl,IWSCONNECTOR*,std::owner_less<websocketpp::connection_hdl>> CEnableContext;
		CEnableContext _enabled_map;
	};
}
