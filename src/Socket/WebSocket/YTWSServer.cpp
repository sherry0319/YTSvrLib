#include "stdafx.h"
#include "YTWSServer.h"
#pragma warning( push )
#pragma warning(disable:4273)
#include "libwebsockets.h"
#pragma warning( pop )
#include <openssl/ssl.h>

namespace YTSvrLib
{
	IWSSERVER::IWSSERVER()
	{
		_port = 0;
		_core = NULL;
	}

	IWSSERVER::~IWSSERVER()
	{
		StopThread();

		lws_context_destroy(_core);

		_core = NULL;
	}

	int IWSSERVER::GetHeader(void* ctx, lws_token_indexes index, char* out, int len)
	{
		return lws_hdr_copy((lws*) ctx, out, len, index);
	}

	void IWSSERVER::AddHeader(void* ctx, void* indata, const char* name, const char* value)
	{
		lws_process_html_args *args = (lws_process_html_args*) indata;

		int res = lws_add_http_header_by_name((lws*) ctx, (unsigned char*) name,
									(unsigned char*) value, (int) strlen(value),
									(unsigned char**) &args->p,
									(unsigned char*) args->p + args->max_len);
		if (res)
		{
			LOG("Add WebSocket Header Failed : %d",res);
		}
	}

	int IWSSERVER::Send(void* ctx, const char* msg, int len, lws_write_protocol type /*= LWS_WRITE_TEXT*/)
	{
		return lws_write((lws*) ctx, (unsigned char*)msg, len, type);
	}

	SOCKET IWSSERVER::GetSocket(void* ctx)
	{
		if (ctx)
		{
			return lws_get_socket_fd((lws*) ctx);
		}
		
		return (SOCKET) 0;
	}

	void IWSSERVER::GetIPAddr(void* ctx, char* out, int len, int& port)
	{
		if (ctx)
		{
			char client_name[128] = { 0 };
			SOCKET fd = GetSocket(ctx);
			lws_get_peer_addresses((lws*) ctx, fd,
								   client_name, sizeof(client_name),
								   out, len);

			socklen_t slen = sizeof(sockaddr_storage);
			sockaddr_storage addr;

			getpeername(fd, (sockaddr*) &addr, &slen);

			struct sockaddr_in *s = (struct sockaddr_in*)&addr;
			port = ntohs(s->sin_port);
		}
	}

	void IWSSERVER::WaitWritable(void* ctx)
	{
		if (ctx)
		{
			lws_callback_on_writable((lws*) ctx);
		}
	}

	bool IWSSERVER::OnSend(void* ctx)
	{
		IWSCONNECTOR* connector = GetConnector(ctx);
		if (connector)
		{
			connector->OnSend();

			return true;
		}

		return false;
	}

	// 注意,会从不同线程调入.加锁!
	IWSCONNECTOR* IWSSERVER::GetConnector(void* ctx)
	{
		IWSCONNECTOR* connector = NULL;
		_connector_lock.Lock();
		auto it = _enabled_map.find(ctx);
		if (it != _enabled_map.end())
		{
			connector = it->second;
		}
		_connector_lock.UnLock();

		return connector;
	}

	// 注意,会从不同线程调入.加锁!
	void IWSSERVER::SetContextEnable(void* ctx, IWSCONNECTOR* connectior, bool enable)
	{
		_connector_lock.Lock();
		if (enable && connectior)
		{
			_enabled_map[ctx] = connectior;
		}
		else
		{
			_enabled_map.erase(ctx);
		}
		_connector_lock.UnLock();
	}

	int IWSSERVER::lws_service_callback(lws *wsi, lws_callback_reasons reason, void *user, void* indata, size_t len)
	{// https://libwebsockets.org/lws-api-doc-master/html/group__usercb.html
		if (reason == LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS ||
			reason == LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS ||
			reason == LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION ||
			reason == LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION)
		{
			return 0;
		}
		IWSSESSION* psd = (IWSSESSION*) user;
		IWSSERVER* server = (IWSSERVER*) lws_get_protocol(wsi)->user;
		if (server && server->isDebug())
		{
			LOG("lws_service_callback = %x server=%x reason=%d", user, server, reason);
		}
		switch (reason)
		{
			case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
			{// 最先来.用户检验是否允许连接
				psd->socket = GetSocket(wsi);
				GetIPAddr(wsi, psd->ip, 31,psd->port);

				if (server->OnClientPreConnect(wsi, psd))
				{
					return 0;
				}
				return -1;
			}break;
			case LWS_CALLBACK_ADD_HEADERS:
			{// 添加自定义头文件
				AddHeader(wsi, indata, "Server:", "Gametrend WebSocket Server");
			}break;
			case LWS_CALLBACK_ESTABLISHED:
			{// 连接已成功.握手结束
				server->OnClientConnected(wsi, psd);
			}break;
			case LWS_CALLBACK_RECEIVE:
			{// 收到消息
				server->OnClientRecvedData(wsi, psd, (char*) indata, len);
			}break;
			case LWS_CALLBACK_CLOSED:
			{// 准备关闭连接
				LOG("LWS_CALLBACK_CLOSED => ctx=%x psd=%x",wsi,psd);
				server->OnClientDisconnect(wsi, psd);
			}break;
			case LWS_CALLBACK_WSI_DESTROY:
			{// 连接已关闭.准备销毁连接数据和用户数据
				LOG("LWS_CALLBACK_WSI_DESTROY => ctx=%x psd=%x",wsi,psd);
			}break;
			case LWS_CALLBACK_SERVER_WRITEABLE:{
				// 这个功能也用作主动关闭连接.
				// 在不调用lws_callback_on_writable之前不会走到这里来.
				// 调用过lws_callback_on_writable之后在下一个循环中就会走到这里来
				// 见:https://libwebsockets.org/pipermail/libwebsockets/2013-September/000542.html
				// 见:https://libwebsockets.org/lws-api-doc-master/html/group__usercb.html 中对 LWS_CALLBACK_CLIENT_WRITEABLE 的解释
				if (!server->OnSend(wsi))
				{
					LOG("LWS_CALLBACK_SERVER_WRITEABLE CLOSE BY SERVER => ctx=%x psd=%x", wsi, psd);
					return -1;
				}
			}break;
			default:
				break;
		}

		return 0;
	}

	bool IWSSERVER::OnClientPreConnect(void* , IWSSESSION* ) // 当客户端即将连接成功.返回true则允许连接.返回false则拒绝连接
	{
		return true;// 允许所有人连接
	}

	void IWSSERVER::OnClientConnected(void* ctx, IWSSESSION* session)
	{
		postWSEvent(ctx, session, WSEType_ClientAccept);
	}

	void IWSSERVER::OnClientRecvedData(void* ctx, IWSSESSION* session, char* msg, size_t len)
	{
		postWSMsg(ctx, session, msg, (int) len);
	}

	void IWSSERVER::OnClientDisconnect(void* ctx, IWSSESSION* session)
	{
		SetContextEnable(ctx,NULL,false);
		postWSEvent(ctx, session, WSEType_ClientClose);
	}

	lws_protocols IWSSERVER::_protocols[2] = {
		/* first protocol must always be HTTP handler */

		{
			"request",    /* name */
			lws_service_callback,    /* callback */
			sizeof(IWSSESSION),  /* per_session_data_size */
			0,      /* max frame size / rx buffer */
		},
		{ NULL, NULL, 0, 0 } /* terminator */
	};

	BOOL IWSSERVER::StartListen(int nPort, const char* ssl_cert_filepath /*= NULL*/, const char* ssl_private_key_filepath /*= NULL*/, BOOL debug /*= false*/)
	{
		_port = nPort;

		if (CreateEvent() == FALSE)
		{
			return FALSE;
		}

		LOG("listening port=%d ssl_cert_filepath=%s ssl_private_key_filepath=%s",nPort,ssl_cert_filepath,ssl_private_key_filepath);
		if (ssl_cert_filepath)
		{
			if (strlen(ssl_cert_filepath) == 0)
			{
				ssl_cert_filepath = NULL;
			}
		}
		if (ssl_private_key_filepath)
		{
			if (strlen(ssl_private_key_filepath) == 0)
			{
				ssl_private_key_filepath = NULL;
			}
		}

		_protocols[0].user = this;

		lws_context_creation_info info;
		ZeroMemory(&info, sizeof(info));
		info.port = _port;
		info.protocols = _protocols;
		info.extensions = NULL;
		info.ssl_cert_filepath = ssl_cert_filepath;
		info.ssl_private_key_filepath = ssl_private_key_filepath;
		info.gid = -1;
		info.uid = -1;
		info.max_http_header_pool = 1;
		info.options = LWS_SERVER_OPTION_LIBEVENT | LWS_SERVER_OPTION_DISABLE_IPV6;
		if (ssl_cert_filepath && ssl_private_key_filepath)
		{
			info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
			info.options |= LWS_SERVER_OPTION_ALLOW_NON_SSL_ON_SSL_PORT;
		}

		_core = lws_create_context(&info);
		if (_core == NULL)
		{
			return FALSE;
		}

		// Don't use the default Signal Event Watcher & Handler
		lws_event_sigint_cfg(_core, 0, NULL);
		// Initialize the LWS with libevent loop
		lws_event_initloop(_core, GetEvent(), 0);
		_debug = debug;
		if (debug)
		{
			/* tell the library what debug level to emit and to send it to syslog */
			int debug_level = LLL_INFO | LLL_DEBUG | LLL_WARN | LLL_ERR | LLL_NOTICE;
			lws_set_log_level(debug_level, IWSSERVER::lws_catch_syslog);
		}
		else
		{
			lws_set_log_level(0, IWSSERVER::lws_catch_syslog);
		}

		CreateThread();

		return TRUE;
	}

	void IWSSERVER::lws_catch_syslog(int level, const char *line)
	{
		LOG("LWS LOG[%d]:%s",level,line);
	}
}