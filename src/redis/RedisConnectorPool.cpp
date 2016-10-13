/*MIT License

Copyright (c) 2016 Zhe Xu

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
#include "stdafx.h"
#include "RedisConnector.h"

namespace YTSvrLib
{

	void CRedisConnectorPool::AddRedisConnect(const char* strIP, int nPort, const char* strAuth)
	{
		CRedisConnector* pNewRedisConnect = new CRedisConnector;

		pNewRedisConnect->SetConnectInfo(strIP, nPort, strAuth);

		m_vctRedisPool.push_back(pNewRedisConnect);
	}

#ifdef LIB_WINDOWS
	unsigned WINAPI CRedisConnectorPool::ThreadRedisAsync(LPVOID pParam)
	{
		CRedisConnectorPool* pThis = (CRedisConnectorPool*) pParam;

		if (pThis->GetEventBase())
		{
			event_base_dispatch(pThis->GetEventBase());
		}

		return 0;
	}
#else
	void* CRedisConnectorPool::ThreadRedisAsync(LPVOID pParam)
	{
		pthread_detach(pthread_self());

		CRedisConnectorPool* pThis = (CRedisConnectorPool*) pParam;

		if (pThis->GetEventBase())
		{
			event_base_dispatch(pThis->GetEventBase());
		}

		return NULL;
	}
#endif // LIB_WINDOWS

	void CRedisConnectorPool::StartRedisConnect(REDIS_HASH_FUNCTION pFunc)
	{
		if (pFunc)
		{
			m_pHashFunction = pFunc;
		}

		if (m_pAsyncRedisEventBase == NULL)
		{
			m_pAsyncRedisEventBase = event_init();
			if (m_pAsyncRedisEventBase == NULL)
			{
				LOG("Create Event Base Error");
				return;
			}
		}

		for (size_t i = 0; i < m_vctRedisPool.size(); ++i)
		{
			if (m_vctRedisPool[i])
			{
				m_vctRedisPool[i]->SetID((int) i);
				m_vctRedisPool[i]->StartRedisConnect();
			}
		}

#ifdef LIB_WINDOWS
		m_hThread = (HANDLE) _beginthreadex(NULL,       // Security
											0,						// Stack size - use default
											ThreadRedisAsync,     	// Thread fn entry point
											(void*)this,				// Param for thread
											0,						// Init flag
											0);			// Thread address
#else
		pthread_create(&m_hThread, NULL, ThreadRedisAsync, this);
#endif // LIB_WINDOWS
	}

	CRedisConnector* CRedisConnectorPool::GetRedisConnector(int nID)
	{
		if (nID < 0 || nID >= (int) m_vctRedisPool.size())
		{
			return NULL;
		}

		return m_vctRedisPool[nID];
	}

	CRedisConnector* CRedisConnectorPool::GetRedisConnector(const redisAsyncContext* p)
	{
		for (size_t i = 0; i < m_vctRedisPool.size(); ++i)
		{
			if (m_vctRedisPool[i] && m_vctRedisPool[i]->IsYourAsyncObject(p))
			{
				return m_vctRedisPool[i];
			}
		}

		return NULL;
	}

	void CRedisConnectorPool::OnAsyncRedisConnected(const redisAsyncContext *c, int status)
	{
		CRedisConnector* pConnector = GetInstance()->GetRedisConnector(c);
		if (pConnector)
		{
			pConnector->OnAysncRedisConnected(status);
		}
	}

	void CRedisConnectorPool::OnAsyncRedisDisconnected(const redisAsyncContext *c, int status)
	{
		CRedisConnector* pConnector = GetInstance()->GetRedisConnector(c);
		if (pConnector)
		{
			pConnector->OnAysncRedisDisconnected(status);
		}
	}

	void CRedisConnectorPool::OnAsyncRedisAuthReply(redisAsyncContext *, void *reply, void *privdata)
	{
		CRedisConnector* pConnector = (CRedisConnector*) privdata;
		if (pConnector)
		{
			pConnector->OnAysncRedisAuth((redisReply*) reply);
		}

		freeReplyObject(reply);
	}

	void CRedisConnectorPool::OnAsyncRedisReply(redisAsyncContext *, void *reply, void *privdata)
	{
		CRedisConnector* pConnector = (CRedisConnector*) privdata;
		if (pConnector)
		{
			pConnector->OnAysncRedisReply((redisReply*) reply);
		}

		freeReplyObject(reply);
	}

	BOOL CRedisConnectorPool::ExcuteCommand(int nHash, const char* pszCmd, char* pszOut, int nMaxLen)
	{
		int nID = 0;
		if (m_pHashFunction)
		{
			nID = m_pHashFunction(nHash);
		}

		CRedisConnector* pConnector = GetRedisConnector(nID);
		if (pConnector == NULL)
		{
			LOG("Invalid Redis Connector Key : %d->%d", nHash, nID);
			return FALSE;
		}

		return pConnector->ExcuteCommand(pszCmd, pszOut, nMaxLen);
	}

	BOOL CRedisConnectorPool::ExcuteCommand(int nHash, const char* pszCmd, std::string& strOut)
	{
		int nID = 0;
		if (m_pHashFunction)
		{
			nID = m_pHashFunction(nHash);
		}

		CRedisConnector* pConnector = GetRedisConnector(nID);
		if (pConnector == NULL)
		{
			LOG("Invalid Redis Connector Key : %d->%d", nHash, nID);
			return FALSE;
		}

		return pConnector->ExcuteCommand(pszCmd, strOut);
	}

	BOOL CRedisConnectorPool::ExcuteCommand(int nHash, const char* pszCmd, int& nOut)
	{
		int nID = 0;
		if (m_pHashFunction)
		{
			nID = m_pHashFunction(nHash);
		}

		CRedisConnector* pConnector = GetRedisConnector(nID);
		if (pConnector == NULL)
		{
			LOG("Invalid Redis Connector Key : %d->%d", nHash, nID);
			return FALSE;
		}

		return pConnector->ExcuteCommand(pszCmd, nOut);
	}

	BOOL CRedisConnectorPool::ExcuteAsyncCommand(int nHash, const char* pszCmd)
	{
		int nID = 0;
		if (m_pHashFunction)
		{
			nID = m_pHashFunction(nHash);
		}

		CRedisConnector* pConnector = GetRedisConnector(nID);
		if (pConnector == NULL)
		{
			LOG("Invalid Redis Connector Key : %d->%d", nHash, nID);
			return FALSE;
		}

		return pConnector->ExcuteAsyncCommand(pszCmd);
	}

	BOOL CRedisConnectorPool::ExcuteCommand(int nHash, const char* pszCmd, std::vector<std::string>& vctOuts)
	{
		int nID = 0;
		if (m_pHashFunction)
		{
			nID = m_pHashFunction(nHash);
		}

		CRedisConnector* pConnector = GetRedisConnector(nID);
		if (pConnector == NULL)
		{
			LOG("Invalid Redis Connector Key : %d->%d", nHash, nID);
			return FALSE;
		}

		return pConnector->ExcuteCommand(pszCmd, vctOuts);
	}

	char CRedisConnectorPool::m_szPublicKeyTemp[1024 * 8] = { 0 };

	const char* CRedisConnectorPool::MakeKey(const char* fmt, ...)
	{
		ZeroMemory(m_szPublicKeyTemp, sizeof(m_szPublicKeyTemp));

		va_list va;
		va_start(va, fmt);
		_vsnprintf_s(m_szPublicKeyTemp, (1024 * 8), fmt, va);
		va_end(va);

		return m_szPublicKeyTemp;
	}
}