#pragma once
#ifndef __YTHTTP_BASE_
#define __YTHTTP_BASE_

#include <event.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>

#define MYHTTPD_SIGNATURE "GAMETREND_HTTP_SERVER via libevent"

namespace YTSvrLib
{
	const char* _GET(struct evhttp_request *req, const char* key);

	const char* _PARAM(struct evkeyvalq *params, const char* key);

	const char* _POST(struct evhttp_request *req, const char* key);

	class IHTTPSERVER : public ITCPEVENTTHREAD
	{
	public:
		IHTTPSERVER()
		{
			m_pHttpCore = NULL;
			m_nListenPort = 0;
		}

		virtual ~IHTTPSERVER()
		{

		}

		virtual void Clean()
		{
			ITCPEVENTTHREAD::CleanThread();
			m_pHttpCore = NULL;
			m_nListenPort = 0;
		}

		static void OnHTTPRequest(evhttp_request *req, void *arg);

		BOOL CreateHTTPSvr(USHORT nPortListen);

		virtual void OnHTTPRequest(evhttp_request *req) = 0;

		evhttp* GetHttpCore()
		{
			return m_pHttpCore;
		}

		void Send(evhttp_request *req,const char* pszData, int nLen);

		void SendHttpErrorMsg(evhttp_request *req,int nCode,const char* msg);
	private:
		int m_nListenPort;
		evhttp* m_pHttpCore;
	};
}

#endif