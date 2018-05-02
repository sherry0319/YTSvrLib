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

#include "YTWSConnector.h"

namespace YTSvrLib
{
	struct IWSSESSION
	{
		SOCKET socket;
		char ip[32];
		int port;
	};

	class YTSVRLIB_EXPORT IWSSERVER : public ITCPEVENTTHREAD, public CWSParserBase
	{
	public:
		IWSSERVER();
		virtual ~IWSSERVER();
		BOOL isDebug()
		{
			return _debug;
		}
	public:
		BOOL StartListen(int nPort, const char* ssl_cert_filepath = NULL, const char* ssl_private_key_filepath = NULL, BOOL debug = FALSE);
	public:
		virtual bool OnClientPreConnect(void* ctx, IWSSESSION* session); // 当客户端即将连接成功.返回true则允许连接.返回false则拒绝连接

		virtual void OnClientConnected(void* ctx, IWSSESSION* session); 

		virtual void OnClientRecvedData(void* ctx, IWSSESSION* session, char* msg, size_t len);

		virtual void OnClientDisconnect(void* ctx, IWSSESSION* session);

		// 实际发送数据(注意,在不同的线程里注意加锁.不是我要这样做的.libwebsockets要求发送者线程不能和lws_callback在不同的线程)
		// 返回true则正常继续.返回false连接断开
		virtual bool OnSend(void* ctx);

		// 注意,会从不同线程调入.加锁!
		virtual IWSCONNECTOR* GetConnector(void* ctx);

		// 注意,会从不同线程调入.加锁!
		virtual void SetContextEnable(void* ctx,IWSCONNECTOR* connectior,bool enable);
	public:
		static int GetHeader(void* ctx, lws_token_indexes index, char* out, int len);
	protected:
		static int Send(void* ctx, const char* msg, int len, lws_write_protocol type = LWS_WRITE_TEXT);

		static SOCKET GetSocket(void* ctx);

		static void GetIPAddr(void* ctx, char* out, int len , int& port);

		static void WaitWritable(void* ctx);
	private:
		static void AddHeader(void* ctx, void* indata, const char* name, const char* value);

		static int lws_service_callback(lws *wsi, lws_callback_reasons reason, void *user, void* data, size_t len);

		static void lws_catch_syslog(int level, const char *line);
	private:
		friend void IWSCONNECTOR::OnSend();
		friend void IWSCONNECTOR::Close();
		friend void IWSCONNECTOR::WaitWritable();

		static lws_protocols _protocols[2];
		int _port;
		BOOL _debug;
		lws_context* _core;

		YTSvrLib::CLock _connector_lock;
		typedef std::unordered_map<void*,IWSCONNECTOR*> CEnableContext;
		CEnableContext _enabled_map;
	};
}
