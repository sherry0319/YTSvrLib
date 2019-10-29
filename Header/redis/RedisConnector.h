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
#ifndef __REDIS_CONNECTOR_H_
#define __REDIS_CONNECTOR_H_

// 获取哈希值的函数指针

#ifdef YTSVRLIB_WITH_REDIS
#include "cpp_redis/cpp_redis"
#endif // YTSVRLIB_WITH_REDIS

namespace YTSvrLib
{
	typedef int(*REDIS_HASH_FUNCTION)(int);

#ifdef YTSVRLIB_WITH_REDIS
	class YTSVRLIB_EXPORT CRedisConnector
	{
	public:
		CRedisConnector():m_Core()
		{
			ZeroMemory(m_szIP, sizeof(m_szIP));
			ZeroMemory(m_szAuthPassword, sizeof(m_szAuthPassword));
			m_nPort = 0;

			m_nID = 0;
			m_bInited = FALSE;
			m_bConnected = FALSE;
		}

		virtual ~CRedisConnector()
		{
			m_Core.disconnect(true);
		}

		void SetConnectInfo(const char* strIP, int nPort, const char* strAuth);
		void StartRedisConnect();

		void SetID(int nID)
		{
			m_nID = nID;
		}
		int GetID()
		{
			return m_nID;
		}

		cpp_redis::client& GetRedisCore() {
			return m_Core;
		}
	protected:
		cpp_redis::client m_Core;

		int m_nID;

		char m_szIP[64];
		char m_szAuthPassword[128];
		int m_nPort;

		BOOL m_bInited;
		BOOL m_bConnected;
	};

	class YTSVRLIB_EXPORT CRedisConnectorPool : public CSingle < CRedisConnectorPool >
	{
	public:
		CRedisConnectorPool()
		{
			m_pHashFunction = NULL;
			m_vctRedisPool.clear();
		}

		virtual ~CRedisConnectorPool()
		{

		}
	public:
		// 设置调用redis时的对键值的哈希函数.REDIS_HASH_FUNCTION 原型为 int REDIS_HASH_FUNCTION(int nKey).
		void SetHashFunction(REDIS_HASH_FUNCTION pFunc)
		{
			m_pHashFunction = pFunc;
		}
		/* 配置增加一个redis连接.每个连接都包含一个同步连接和一个异步连接.所有的异步连接都共用一个专门线程.
		注意这只是增加一个配置.并不连接.需要都配置完成后调用StartRedisConnect来启动*/
		void AddRedisConnect(const char* strIP, int nPort, const char* strAuth);
		/* 启动redis连接.
		pFunc : 调用redis时的对键值的哈希函数.默认为NULL.为NULL则所有的命令都发往第一个redis库
		REDIS_HASH_FUNCTION 原型为 int REDIS_HASH_FUNCTION(int nKey).*/
		void StartRedisConnect(REDIS_HASH_FUNCTION pFunc = NULL);
	public:
		cpp_redis::client& GetRedisConnectorByKey(int nParam);
	protected:
		cpp_redis::client& GetRedisConnector(int nID);
		vector<CRedisConnector*> m_vctRedisPool;
		REDIS_HASH_FUNCTION m_pHashFunction;
	};
	
#endif
}

#endif // !__REDIS_CONNECTOR_H_
