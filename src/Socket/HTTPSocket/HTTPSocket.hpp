#pragma once

#ifndef __HTTP_SOCKET_H_
#define __HTTP_SOCKET_H_

#ifdef YTSVRLIB_WITH_HTTP

#ifndef USE_STANDALONE_ASIO
#define USE_STANDALONE_ASIO 1
#endif // !USE_STANDALONE_ASIO

#include "Lib/server_https.hpp"
#include "../YTSocketThread.h"
#endif // YTSVRLIB_WITH_HTTP

namespace YTSvrLib {

#ifdef YTSVRLIB_WITH_HTTP
	enum EM_HTTP_METHOD {
		METHOD_HEAD,
		METHOD_GET,
		METHOD_POST,

		METHOD_OPTIONS,
		METHOD_PUT,
		METHOD_DELETE,
	};

	using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
	using HttpsServer = SimpleWeb::Server<SimpleWeb::HTTPS>;
	
	template<class _SERVER_TYPE>
	class YTSVRLIB_EXPORT IHTTPBASE : public IASIOTHREAD {
	public:
		using Server = _SERVER_TYPE;
		using Responce = std::shared_ptr<typename Server::Response>;
		using Request = std::shared_ptr<typename Server::Request>;
		using Handler = std::function<void(Responce, Request)>;
		using HTTPParams = std::unordered_map<std::string, std::string>;
	public:
		struct MSGITEM : public CRecycle
		{
			MSGITEM() {
				_req = NULL;
				_res = NULL;
			}

			virtual void Init() {
				_req = NULL;
				_res = NULL;
			}

			Request _req;
			Responce _res;
		};

		virtual void SetEvent() = 0;

		void AddNewMessage(Responce res, Request req) {
			auto* pNewItem = m_poolMsgs.ApplyObj();
			if (pNewItem)
			{
				pNewItem->_req = req;
				pNewItem->_res = res;

				BOOL bSetEvent = FALSE;
				m_lockMsg.Lock();
				if (m_queueMsgs.empty())
				{
					bSetEvent = TRUE;
				}
				m_queueMsgs.push_back(pNewItem);
				m_lockMsg.UnLock();

				if (bSetEvent)
				{
					SetEvent();
				}
			}
		}

		int GetQueueSize() {
			return (int)m_queueMsgs.size();
		}

		virtual void MessageConsumer() {
			if (m_queueMsgs.empty())
			{
				return;
			}

			int nCount = 0;
			BOOL bNeedSetEvt = FALSE;
			do
			{
				if (nCount >= 128)
				{	//避免其他事件队列堵塞
					bNeedSetEvt = TRUE;
					break;
				}

				m_lockMsg.Lock();
				if (m_queueMsgs.empty())
				{
					m_lockMsg.UnLock();
					break;
				}
				MSGITEM* pItem = m_queueMsgs.front();
				m_queueMsgs.pop_front();
				m_lockMsg.UnLock();

				if (pItem)
				{
					ProcessMessage(pItem->_res, pItem->_req);
				}
			} while (true);

			if (bNeedSetEvt)
			{
				SetEvent();
			}
		}

		// 处理数据消息
		virtual void ProcessMessage(Responce res,Request req) = 0;
	protected:
		std::list<MSGITEM*> m_queueMsgs;
		CPool<MSGITEM, 128> m_poolMsgs;
		CLock m_lockMsg;
	public:
		IHTTPBASE() : m_poolMsgs("HTTPMSGITEM") {
		}

		BOOL StartListen(int nPort) {
			m_Core->config.port = (unsigned short)nPort;
#ifdef LIB_WINDOWS
			m_Core->config.reuse_address = false;
#endif // LIB_WINDOWS

			m_Core->config.timeout_content = 0;

			static std::map<EM_HTTP_METHOD, std::string> sHttpMethod = {
				{METHOD_GET,"GET"},
				{METHOD_POST,"POST"},
				{METHOD_HEAD,"HEAD"},
				{METHOD_PUT,"PUT"},
				{METHOD_OPTIONS,"OPTIONS"},
				{METHOD_DELETE,"DELETE"}
			};

			for (auto& p : sHttpMethod)
			{
				m_Core->default_resource[p.second] = [this](Responce res, Request req) {
					AddNewMessage(res, req);
				};
			}

			try {
				m_Core->bind();

				CreateThread("IHTTPBASE");
				
				return TRUE;
			}
			catch (std::exception& ec) {
				LOG("Http Server Start Listen Error : [%s]",ec.what());
				return FALSE;
			}

			return FALSE;
		}

		virtual void EventLoop() override {
			m_bRun = TRUE;

			m_Core->accept_and_run();
		}
	protected:
		std::shared_ptr<Server> m_Core;
	};

	template<class _SERVER_TYPE>
	class YTSVRLIB_EXPORT HTTPSERVER : public IHTTPBASE<_SERVER_TYPE> {

	public:
		HTTPSERVER() {
			IHTTPBASE<_SERVER_TYPE>::m_Core = std::make_shared<_SERVER_TYPE>();
		}
	};

	template<>
	class YTSVRLIB_EXPORT HTTPSERVER<HttpsServer> : public IHTTPBASE<HttpsServer> {
	public:
		HTTPSERVER(const char* cert = "", const char* key = "", const char* ca = "") {
			IHTTPBASE<HttpsServer>::m_Core = std::make_shared<Server>(cert, key, ca);
		}
	};
#endif // YTSVRLIB_WITH_HTTP

}

#endif