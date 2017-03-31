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
#include "stdafx.h"
#include "RedisConnector.h"

namespace YTSvrLib
{

	void CRedisConnector::SetConnectInfo(const char* strIP, int nPort, const char* strAuth)
	{
		if (m_bInited == FALSE)
		{
			m_bInited = TRUE;
			strncpy_s(m_szIP, strIP, 63);
			strncpy_s(m_szAuthPassword, strAuth, 127);
			m_nPort = nPort;
		}
	}

	void CRedisConnector::StartRedisConnect()
	{
		CheckSyncRedisConnect();
		CheckAsyncRedisConnect();
	}

	BOOL CRedisConnector::IsYourAsyncObject(const redisAsyncContext* p)
	{
		if (m_pAsyncRedisConnector == NULL)
		{
			return FALSE;
		}

		if (p == NULL)
		{
			return FALSE;
		}

		if (m_pAsyncRedisConnector != p)
		{
			return FALSE;
		}

		return TRUE;
	}

	void CRedisConnector::OnAysncRedisConnected(int status)
	{
		LOG("OnAysncRedis=%s:%d Connected=%d", m_szIP, m_nPort, status);
	}

	void CRedisConnector::OnAysncRedisDisconnected(int status)
	{
		m_bAsyncRedisConnected = FALSE;
		LOG("OnAysncRedis=%s:%d Disconnected=%d", m_szIP, m_nPort, status);

		CheckAsyncRedisConnect();
	}

	void CRedisConnector::OnAysncRedisAuth(redisReply* reply)
	{
		m_bAsyncRedisConnecting = FALSE;

		if (reply->type == REDIS_REPLY_STATUS && strstr(reply->str, "+OK"))
		{
			m_bAsyncRedisConnected = TRUE;
		}
		else
		{
			m_bAsyncRedisConnected = FALSE;
		}
	}

	void CRedisConnector::OnAysncRedisReply(redisReply* reply)
	{
		LOG("OnAysncRedis=[%d]%s:%d Get Reply : (%d)%s", GetID(), m_szIP, m_nPort, reply->type, reply->str);
	}

	BOOL CRedisConnector::CheckSyncRedisConnect()
	{
		if (m_pSyncRedisConnector && m_pSyncRedisConnector->fd != INVALID_SOCKET && (m_pSyncRedisConnector->flags & REDIS_CONNECTED) && m_bSyncRedisConnected)
		{
			return TRUE;
		}

		m_bSyncRedisConnected = FALSE;

		if (m_pSyncRedisConnector)
		{
			redisFree(m_pSyncRedisConnector);
			m_pSyncRedisConnector = NULL;
		}

		const timeval tv = { 1, 0 };//1秒以后超时.避免被同步select阻塞死

		m_pSyncRedisConnector = redisConnectWithTimeout(m_szIP, m_nPort, tv);
		if (m_pSyncRedisConnector == NULL)
		{
			LOG("Connect Sync Redis Failed : %s:%d", m_szIP, m_nPort);
			return FALSE;
		}

		if (strlen(m_szAuthPassword) > 0)
		{
			redisReply* pAuthReply = (redisReply*) redisCommand(m_pSyncRedisConnector, "AUTH %s", m_szAuthPassword);
			if (pAuthReply == NULL || strstr(pAuthReply->str, "+OK") == NULL)
			{
				LOG("Redis Connect Error : redis auth password failed");
				return FALSE;
			}
			freeReplyObject(pAuthReply);
		}

		m_bSyncRedisConnected = TRUE;

		return TRUE;
	}
	BOOL CRedisConnector::CheckAsyncRedisConnect()
	{
		if (m_pAsyncRedisConnector && m_pAsyncRedisConnector->c.fd != INVALID_SOCKET && (m_pAsyncRedisConnector->c.flags & REDIS_CONNECTED) && (m_bAsyncRedisConnected || m_bAsyncRedisConnecting))
		{
			return TRUE;
		}

		m_bAsyncRedisConnected = FALSE;

		if (m_pAsyncRedisConnector)
		{
			redisAsyncFree(m_pAsyncRedisConnector);
			m_pAsyncRedisConnector = NULL;
		}

		event_base* pEventBase = CRedisConnectorPool::GetInstance()->GetEventBase();
		if (pEventBase == NULL)
		{
			LOG("Redis Connect Error : event_base null");
			return FALSE;
		}

		m_pAsyncRedisConnector = redisAsyncConnect(m_szIP, m_nPort);
		if (m_pAsyncRedisConnector == NULL)
		{
			LOG("Redis Async Connect Error : redisAsyncConnect failed");
			return FALSE;
		}

		redisLibeventAttach(m_pAsyncRedisConnector, pEventBase);
		redisAsyncSetConnectCallback(m_pAsyncRedisConnector, CRedisConnectorPool::OnAsyncRedisConnected);
		redisAsyncSetDisconnectCallback(m_pAsyncRedisConnector, CRedisConnectorPool::OnAsyncRedisDisconnected);

		if (strlen(m_szAuthPassword) > 0)
		{
			m_bAsyncRedisConnecting = TRUE;
			redisAsyncCommand(m_pAsyncRedisConnector, CRedisConnectorPool::OnAsyncRedisAuthReply, this, "AUTH %s", m_szAuthPassword);
		}
		else
		{
			m_bAsyncRedisConnecting = FALSE;
			m_bAsyncRedisConnected = TRUE;
		}

		return TRUE;
	}

	BOOL CRedisConnector::ExcuteCommand(const char* pszCmd, char* pszOut, int nMaxLen)
	{
		if (m_pSyncRedisConnector == NULL || m_bSyncRedisConnected == FALSE)
		{
			return FALSE;
		}

		ZeroMemory(pszOut, nMaxLen);

		redisReply* reply = (redisReply*) redisCommand(m_pSyncRedisConnector, pszCmd);
		if (reply == NULL)
		{
			LOG("redis Command Excute Failed : %s", pszCmd);
			return FALSE;
		}

		if (reply->len >= nMaxLen)
		{
			LOG("redis Command Overflow = %d/%d", nMaxLen, reply->len);
			freeReplyObject(reply);
			return FALSE;
		}

		if (reply->type == REDIS_REPLY_NIL)
		{
			freeReplyObject(reply);
			return TRUE;
		}

		if (reply->type == REDIS_REPLY_ERROR)
		{
			LOG("redis Commond Excute Failed : %s.Error : %s", pszCmd, reply->str);
			freeReplyObject(reply);
			return FALSE;
		}

		if (reply->type != REDIS_REPLY_STATUS && reply->type != REDIS_REPLY_STRING)
		{
			LOG("redis Command Diff Type : 1,5 <=> %d", reply->type);
			freeReplyObject(reply);
			return FALSE;
		}

#ifdef LIB_WINDOWS
		strncpy_s(pszOut, nMaxLen, reply->str, reply->len);
#else
		strncpy_s(pszOut, reply->str, nMaxLen);
#endif // LIB_WINDOWS

		freeReplyObject(reply);

		return TRUE;
	}

	BOOL CRedisConnector::ExcuteCommand(const char* pszCmd, std::string& strOut)
	{
		strOut.clear();

		if (m_pSyncRedisConnector == NULL || m_bSyncRedisConnected == FALSE)
		{
			return FALSE;
		}

		redisReply* reply = (redisReply*) redisCommand(m_pSyncRedisConnector, pszCmd);
		if (reply == NULL)
		{
			LOG("redis Command Excute Failed : %s", pszCmd);
			return FALSE;
		}

		if (reply->type == REDIS_REPLY_NIL)
		{
			freeReplyObject(reply);
			return TRUE;
		}

		if (reply->type == REDIS_REPLY_ERROR)
		{
			LOG("redis Commond Excute Failed : %s.Error : %s", pszCmd, reply->str);
			freeReplyObject(reply);
			return FALSE;
		}

		if (reply->type != REDIS_REPLY_STATUS && reply->type != REDIS_REPLY_STRING)
		{
			LOG("redis Command Diff Type : 1,5 <=> %d", reply->type);
			freeReplyObject(reply);
			return FALSE;
		}

		strOut = reply->str;

		freeReplyObject(reply);

		return TRUE;
	}

	BOOL CRedisConnector::ExcuteCommand(const char* pszCmd, int& nOut)
	{
		nOut = 0;

		if (m_pSyncRedisConnector == NULL || m_bSyncRedisConnected == FALSE)
		{
			return FALSE;
		}

		redisReply* reply = (redisReply*) redisCommand(m_pSyncRedisConnector, pszCmd);
		if (reply == NULL)
		{
			LOG("redis Command Excute Failed : %s", pszCmd);
			return FALSE;
		}

		if (reply->type == REDIS_REPLY_NIL)
		{
			freeReplyObject(reply);
			return TRUE;
		}

		if (reply->type == REDIS_REPLY_ERROR)
		{
			LOG("redis Commond Excute Failed : %s.Error : %s", pszCmd, reply->str);
			freeReplyObject(reply);
			return FALSE;
		}

		if (reply->type != REDIS_REPLY_INTEGER)
		{
			LOG("redis Command Diff Type : 3 <=> %d", reply->type);
			freeReplyObject(reply);
			return FALSE;
		}

		nOut = reply->len;

		freeReplyObject(reply);

		return TRUE;
	}

	BOOL CRedisConnector::ExcuteCommand(const char* pszCmd, std::vector<std::string>& vctOuts)
	{
		vctOuts.clear();

		if (m_pSyncRedisConnector == NULL || m_bSyncRedisConnected == FALSE)
		{
			return FALSE;
		}

		redisReply* reply = (redisReply*) redisCommand(m_pSyncRedisConnector, pszCmd);
		if (reply == NULL)
		{
			LOG("redis Command Excute Failed : %s", pszCmd);
			return FALSE;
		}

		if (reply->type == REDIS_REPLY_NIL)
		{
			freeReplyObject(reply);
			return TRUE;
		}

		if (reply->type == REDIS_REPLY_ERROR)
		{
			LOG("redis Commond Excute Failed : %s.Error : %s", pszCmd, reply->str);
			freeReplyObject(reply);
			return FALSE;
		}

		if (reply->type != REDIS_REPLY_ARRAY)
		{
			LOG("redis Command Diff Type : 2 <=> %d", reply->type);
			freeReplyObject(reply);
			return FALSE;
		}

		for (size_t i = 0; i < reply->elements; ++i)
		{
			redisReply* cur = reply->element[i];
			if (cur)
			{
				vctOuts.push_back(std::string(cur->str));
			}
		}

		freeReplyObject(reply);

		return TRUE;
	}

	BOOL CRedisConnector::ExcuteAsyncCommand(const char* pszCmd)
	{
		if (m_pAsyncRedisConnector == NULL || m_bAsyncRedisConnected == FALSE)
		{
			return FALSE;
		}

		redisAsyncCommand(m_pAsyncRedisConnector, CRedisConnectorPool::OnAsyncRedisReply, this, pszCmd);

		return TRUE;
	}
}