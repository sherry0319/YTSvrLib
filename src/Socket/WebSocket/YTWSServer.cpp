#include "stdafx.h"
#include "YTWSServer.h"
#include <openssl/ssl.h>

namespace YTSvrLib
{
	IWSSERVER::IWSSERVER()
	{
		_port = 0;
	}

	IWSSERVER::~IWSSERVER()
	{
		StopThread();
	}

	// 注意,会从不同线程调入.加锁!
	IWSCONNECTOR* IWSSERVER::GetConnector(websocketpp::connection_hdl hdl)
	{
		IWSCONNECTOR* connector = NULL;
		_connector_lock.Lock();
		auto it = _enabled_map.find(hdl);
		if (it != _enabled_map.end())
		{
			connector = it->second;
		}
		_connector_lock.UnLock();

		return connector;
	}

	// 注意,会从不同线程调入.加锁!
	void IWSSERVER::SetContextEnable(websocketpp::connection_hdl hdl, IWSCONNECTOR* connectior)
	{
		_connector_lock.Lock();
		_enabled_map[hdl] = connectior;
		_connector_lock.UnLock();
	}

	void IWSSERVER::SetContextDisable(websocketpp::connection_hdl hdl) {
		_connector_lock.Lock();
		_enabled_map.erase(hdl);
		_connector_lock.UnLock();
	}

	BOOL IWSSERVER::StartListen(int nPort, const char* ssl_cert_filepath /*= NULL*/, const char* ssl_private_key_filepath /*= NULL*/, BOOL bDebug /*= FALSE*/)
	{
		_port = nPort;

		LOG("listening port=%d ssl_cert_filepath=%s ssl_private_key_filepath=%s",nPort,ssl_cert_filepath,ssl_private_key_filepath);
		if (ssl_cert_filepath)
		{
			if (strlen(ssl_cert_filepath) > 0)
			{
				_ssl_cert = ssl_cert_filepath;
			}
		}
		if (ssl_private_key_filepath)
		{
			if (strlen(ssl_private_key_filepath) > 0)
			{
				_ssl_key = ssl_private_key_filepath;
			}
		}

		init_asio(GetCore());

		if (bDebug) {
			set_access_channels(websocketpp::log::alevel::all);
		}
		else {
			clear_access_channels(websocketpp::log::alevel::all);
			set_access_channels(websocketpp::log::alevel::none);
		}
		
		// Register our message handler
		set_open_handler(std::bind(&IWSSERVER::onWSConnected,this,std::placeholders::_1));
		set_close_handler(std::bind(&IWSSERVER::onWSClosed, this, std::placeholders::_1));
		set_message_handler(std::bind(&IWSSERVER::onWSDataRecv, this, std::placeholders::_1, std::placeholders::_2));
		set_tls_init_handler(std::bind(&IWSSERVER::onWSTLSInit,this, std::placeholders::_1));
		set_validate_handler(std::bind(&IWSSERVER::onWSValidate, this, std::placeholders::_1));
		
		set_reuse_addr(true);
		// Listen on port 9002
		listen((uint16_t)nPort);

		// Start the server accept loop
		start_accept();

		// Start the ASIO io_service run loop
		CreateThread();

		return TRUE;
	}

	void IWSSERVER::StopListen() {
		stop_listening();
	}

	bool IWSSERVER::onWSValidate(websocketpp::connection_hdl hdl) {
		connection_ptr con = get_con_from_hdl(hdl);

		std::string dstIP = con->get_remote_endpoint();

		return validateClient(dstIP);
	}

	void IWSSERVER::onWSConnected(websocketpp::connection_hdl hdl) {
		connection_ptr con = get_con_from_hdl(hdl);

		IWSCONNECTOR* pConn = AllocateConnector();
		pConn->Create(this, con);

		SetContextEnable(hdl, pConn);
		postWSEvent(pConn,WSEType::WSEType_ClientAccept);
	}

	void IWSSERVER::onWSClosed(websocketpp::connection_hdl hdl) {
		IWSCONNECTOR* pConn = GetConnector(hdl);

		if (pConn)
		{
			postWSEvent(pConn,WSEType::WSEType_ClientClose);

			SetContextDisable(hdl);
		}
	}

	void IWSSERVER::onWSDataRecv(websocketpp::connection_hdl hdl, message_ptr msg) {
		IWSCONNECTOR* pConn = GetConnector(hdl);
		if (pConn)
		{
			auto& info = msg->get_payload();
			
			postWSMsg(pConn, info.c_str(),(int)info.length());
		}
	}

	context_ptr IWSSERVER::onWSTLSInit(websocketpp::connection_hdl hdl) {
		context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);

		try {
			ctx->set_options(asio::ssl::context::default_workarounds |
				asio::ssl::context::no_sslv2 |
				asio::ssl::context::no_sslv3 |
				asio::ssl::context::no_tlsv1 |
				asio::ssl::context::single_dh_use);

			ctx->use_certificate_chain_file(_ssl_cert.c_str());
			ctx->use_private_key_file(_ssl_key.c_str(), asio::ssl::context::pem);
		}
		catch (std::exception& e) {
			LOG("onWSTLSInit Exception : what=%s",e.what());
		}
		return ctx;
	}
}