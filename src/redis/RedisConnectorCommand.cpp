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

	BOOL CRedisConnectorPool::DEL(const char* key, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "DEL %s", key);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::BGSAVE(int nHash /* = 0 */)
	{
		return ExcuteAsyncCommand(nHash, "BGSAVE");
	}

	BOOL CRedisConnectorPool::BGREWRITEAOF(int nHash /* = 0 */)
	{
		return ExcuteAsyncCommand(nHash, "BGREWRITEAOF");
	}

	BOOL CRedisConnectorPool::APPEND(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "APPEND %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::DECRBY(const char* key, int nDec, int& nNew, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "DECRBY %s %d", key, nDec);

		return ExcuteCommand(nHash, szCmd, nNew);
	}

	BOOL CRedisConnectorPool::INCRBY(const char* key, int nInc, int& nNew, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "INCRBY %s %d", key, nInc);

		return ExcuteCommand(nHash, szCmd, nNew);
	}

	BOOL CRedisConnectorPool::EXPIRE(const char* key, __time32_t tSec, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "EXPIRE %s %ld", key, tSec);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::EXPIREAT(const char* key, __time32_t tExpired, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "EXPIREAT %s %ld", key, tExpired);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SET(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SET %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SET(const char* key, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SET %s %d", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SET(const char* key, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SET %s %lld", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SETNX(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SETNX %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SETNX(const char* key, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SETNX %s %d", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SETNX(const char* key, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SETNX %s %lld", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::GET(const char* key, std::string& strValue, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "GET %s", key);

		return ExcuteCommand(nHash, szCmd, strValue);
	}

	BOOL CRedisConnectorPool::GET(const char* key, int& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValue;
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "GET %s", key);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValue);

		if (bRes)
		{
			nValue = atoi(strValue.c_str());
		}

		return nValue;
	}

	BOOL CRedisConnectorPool::GET(const char* key, LONGLONG& nValue, int nHash /*= 0*/)
	{
		nValue = 0;

		std::string strValue;
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "GET %s", key);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValue);

		if (bRes)
		{
			nValue = atoll(strValue.c_str());
		}

		return bRes;
	}

	BOOL CRedisConnectorPool::MGET(const std::vector<std::string>& vctKeys, std::vector<std::string>& vctValues, int nHash /* = 0 */)
	{
		string strCmd = "MGET";

		for (size_t i = 0; i < vctKeys.size(); ++i)
		{
			strCmd += " ";
			strCmd += vctKeys[i];
		}

		return ExcuteCommand(nHash, strCmd.c_str(), vctValues);
	}

	BOOL CRedisConnectorPool::EXISTS(const char* key, BOOL& bExist, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "EXISTS %s", key);

		return ExcuteCommand(nHash, szCmd, bExist);
	}

	BOOL CRedisConnectorPool::TTL(const char* key, __time32_t& tSec, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "TTL %s", key);
		int nSec = 0;

		BOOL bRet = ExcuteCommand(nHash, szCmd, nSec);

		tSec = nSec;

		return bRet;
	}

	BOOL CRedisConnectorPool::KEYS(const char* pattern, std::vector<std::string>& vctKeys, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "KEYS %s", pattern);

		return ExcuteCommand(nHash, szCmd, vctKeys);
	}

	BOOL CRedisConnectorPool::RENAME(const char* key, const char* newkey, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RENAME %s %s", key, newkey);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::RENAMENX(const char* key, const char* newkey, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RENAMENX %s %s", key, newkey);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::STRLEN(const char* key, int& nSize, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "STRLEN %s", key);

		return ExcuteCommand(nHash, szCmd, nSize);
	}

	BOOL CRedisConnectorPool::HDEL(const char* key, const char* field, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HDEL %s %s", key, field);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HEXISTS(const char* key, const char* field, BOOL& bExist, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HEXISTS %s %s", key, field);

		return ExcuteCommand(nHash, szCmd, bExist);
	}

	BOOL CRedisConnectorPool::HGET(const char* key, const char* field, std::string& strValues, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HGET %s %s", key, field);

		return ExcuteCommand(nHash, szCmd, strValues);
	}

	BOOL CRedisConnectorPool::HGET(const char* key, const char* field, int& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValues;

		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HGET %s %s", key, field);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValues);

		nValue = atoi(strValues.c_str());

		return bRes;
	}

	BOOL CRedisConnectorPool::HGET(const char* key, const char* field, LONGLONG& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValues;

		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HGET %s %s", key, field);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValues);

		nValue = atoll(strValues.c_str());

		return bRes;
	}

	BOOL CRedisConnectorPool::HGET(const char* key, int field, std::string& strValues, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HGET %s %d", key, field);

		return ExcuteCommand(nHash, szCmd, strValues);
	}

	BOOL CRedisConnectorPool::HGET(const char* key, int field, int& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValues;

		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HGET %s %d", key, field);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValues);

		nValue = atoi(strValues.c_str());

		return bRes;
	}

	BOOL CRedisConnectorPool::HGET(const char* key, int field, LONGLONG& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValues;

		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HGET %s %d", key, field);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValues);

		nValue = atoll(strValues.c_str());

		return bRes;
	}

	BOOL CRedisConnectorPool::HGETALL(const char* key, std::map<std::string, std::string>& mapValues, int nHash /* = 0 */)
	{
		mapValues.clear();

		vector<string> vctValues;
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HGETALL %s", key);

		BOOL bRes = ExcuteCommand(nHash, szCmd, vctValues);

		if (bRes)
		{
			for (size_t i = 0; i < vctValues.size(); i += 2)
			{
				// filed1 value1 field2 value2 ...
				mapValues[vctValues[i]] = vctValues[i + 1];
			}
		}

		return bRes;
	}

	BOOL CRedisConnectorPool::HINCRBY(const char* key, const char* field, int nInc, int& nNew, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HINCRBY %s %s %d", key, field, nInc);

		return ExcuteCommand(nHash, szCmd, nNew);
	}

	BOOL CRedisConnectorPool::HKEYS(const char* key, std::vector<std::string>& vctFields, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HKEYS %s", key);

		return ExcuteCommand(nHash, szCmd, vctFields);
	}

	BOOL CRedisConnectorPool::HLEN(const char* key, int& nLen, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HLEN %s", key);

		return ExcuteCommand(nHash, szCmd, nLen);
	}

	BOOL CRedisConnectorPool::HMGET(const char* key, const std::vector<std::string>& vctFileds, std::vector<std::string>& vctValues, int nHash /* = 0 */)
	{
		string strCmd = "HMGET ";
		strCmd += key;
		for (size_t i = 0; i < vctFileds.size(); ++i)
		{
			strCmd += " ";
			strCmd += vctFileds[i];
		}

		return ExcuteCommand(nHash, strCmd.c_str(), vctValues);
	}

	BOOL CRedisConnectorPool::HMSET(const char* key, const std::vector<std::string>& vctFileds, const std::vector<std::string>& vctValues, int nHash /* = 0 */)
	{
		string strCmd = "HMSET ";
		strCmd += key;
		for (size_t i = 0; (i < vctFileds.size() && i < vctValues.size()); ++i)
		{
			strCmd += " ";
			strCmd += vctFileds[i];
			strCmd += " ";
			strCmd += vctValues[i];
		}

		return ExcuteAsyncCommand(nHash, strCmd.c_str());
	}

	BOOL CRedisConnectorPool::HMSET(const char* key, const std::vector<int>& vctFileds, const std::vector<std::string>& vctValues, int nHash /*= 0*/)
	{
		string strCmd = "HMSET ";
		strCmd += key;
		char buffer[64] = { 0 };
		for (size_t i = 0; (i < vctFileds.size() && i < vctValues.size()); ++i)
		{
			strCmd += " ";
			_snprintf_s(buffer, 63, "%d", vctFileds[i]);
			strCmd += buffer;
			strCmd += " ";
			strCmd += vctValues[i];
		}

		return ExcuteAsyncCommand(nHash, strCmd.c_str());
	}

	BOOL CRedisConnectorPool::HSET(const char* key, const char* field, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSET %s %s %s", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSET(const char* key, const char* field, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSET %s %s %d", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSET(const char* key, const char* field, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSET %s %s %lld", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSET(const char* key, int field, const char* value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSET %s %d %s", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSET(const char* key, int field, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSET %s %d %d", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSET(const char* key, int field, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSET %s %d %lld", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSETNX(const char* key, const char* field, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSETNX %s %s %s", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSETNX(const char* key, const char* field, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSETNX %s %s %d", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSETNX(const char* key, const char* field, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSETNX %s %s %lld", key, field, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::HSTRLEN(const char* key, const char* field, int& nSize, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "HSTRLEN %s %s", key, field);

		return ExcuteCommand(nHash, szCmd, nSize);
	}

	BOOL CRedisConnectorPool::LINDEX(const char* key, int nIndex, std::string& strValue, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LINDEX %s %d", key, nIndex);

		return ExcuteCommand(nHash, szCmd, strValue);
	}

	BOOL CRedisConnectorPool::LINSERT(const char* key, const char* find, const char* newvalue, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LINSERT %s BEFORE %s %s", key, find, newvalue);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LINSERT(const char* key, const char* find, int newvalue, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LINSERT %s BEFORE %s %d", key, find, newvalue);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LINSERT(const char* key, const char* find, LONGLONG newvalue, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LINSERT %s BEFORE %s %lld", key, find, newvalue);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LLEN(const char* key, int& nSize, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LLEN %s", key);

		return ExcuteCommand(nHash, szCmd, nSize);
	}

	BOOL CRedisConnectorPool::LPOP(const char* key, std::string& strValue, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LPOP %s", key);

		return ExcuteCommand(nHash, szCmd, strValue);
	}

	BOOL CRedisConnectorPool::LPOP(const char* key, int& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValue;

		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LPOP %s", key);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValue);

		if (bRes)
		{
			nValue = atoi(strValue.c_str());
		}

		return bRes;
	}

	BOOL CRedisConnectorPool::LPOP(const char* key, LONGLONG& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValue;

		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LPOP %s", key);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValue);

		if (bRes)
		{
			nValue = atoll(strValue.c_str());
		}

		return bRes;
	}

	BOOL CRedisConnectorPool::LPUSH(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LPUSH %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LPUSH(const char* key, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LPUSH %s %d", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LPUSH(const char* key, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LPUSH %s %lld", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LPUSHX(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LPUSHX %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LRANGE(const char* key, int begin, int end, std::vector<std::string>& vctValues, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LRANGE %s %d %d", key, begin, end);

		return ExcuteCommand(nHash, szCmd, vctValues);
	}

	BOOL CRedisConnectorPool::LREM(const char* key, int count_to_remove, const char* find, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LREM %s %d %s", key, count_to_remove, find);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LSET(const char* key, int index, const char* newvalue, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LSET %s %d %s", key, index, newvalue);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LSET(const char* key, int index, int newvalue, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LSET %s %d %d", key, index, newvalue);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::LSET(const char* key, int index, LONGLONG newvalue, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "LSET %s %d %lld", key, index, newvalue);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::RPOP(const char* key, std::string& strValue, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RPOP %s", key);

		return ExcuteCommand(nHash, szCmd, strValue);
	}

	BOOL CRedisConnectorPool::RPOP(const char* key, int& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValue;

		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RPOP %s", key);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValue);
		if (bRes)
		{
			nValue = atoi(strValue.c_str());
		}

		return bRes;
	}

	BOOL CRedisConnectorPool::RPOP(const char* key, LONGLONG& nValue, int nHash /*= 0*/)
	{
		nValue = 0;
		std::string strValue;

		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RPOP %s", key);

		BOOL bRes = ExcuteCommand(nHash, szCmd, strValue);
		if (bRes)
		{
			nValue = atoll(strValue.c_str());
		}

		return bRes;
	}

	BOOL CRedisConnectorPool::RPOPLPUSH(const char* key, const char* dest, std::string& strValue, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RPOPLPUSH %s %s", key, dest);

		return ExcuteCommand(nHash, szCmd, strValue);
	}

	BOOL CRedisConnectorPool::RPUSH(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RPUSH %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::RPUSH(const char* key, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RPUSH %s %d", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::RPUSH(const char* key, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RPUSH %s %lld", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::RPUSHX(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "RPUSHX %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SADD(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SADD %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SADD(const char* key, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SADD %s %d", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SADD(const char* key, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SADD %s %lld", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SCARD(const char* key, int& nSize, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SCARD %s", key);

		return ExcuteCommand(nHash, szCmd, nSize);
	}

	BOOL CRedisConnectorPool::SDIFF(const char* key1, const char* key2, std::vector<std::string>& vctElements, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SDIFF %s %s", key1, key2);

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::SDIFFSTORE(const char* key1, const char* key2, const char* newkey, int& nNewSize, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SDIFFSTORE %s %s %s", key1, key2, newkey);

		return ExcuteCommand(nHash, szCmd, nNewSize);
	}

	BOOL CRedisConnectorPool::SINTER(const char* key1, const char* key2, std::vector<std::string>& vctElements, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SINTER %s %s", key1, key2);

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::SINTERSTORE(const char* key1, const char* key2, const char* newkey, int& nNewSize, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SINTERSTORE %s %s %s", key1, key2, newkey);

		return ExcuteCommand(nHash, szCmd, nNewSize);
	}

	BOOL CRedisConnectorPool::SISMEMBER(const char* key, const char* value, BOOL& bIsMember, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SISMEMBER %s %s", key, value);

		return ExcuteCommand(nHash, szCmd, bIsMember);
	}

	BOOL CRedisConnectorPool::SISMEMBER(const char* key, int value, BOOL& bIsMember, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SISMEMBER %s %d", key, value);

		return ExcuteCommand(nHash, szCmd, bIsMember);
	}

	BOOL CRedisConnectorPool::SISMEMBER(const char* key, LONGLONG value, BOOL& bIsMember, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SISMEMBER %s %lld", key, value);

		return ExcuteCommand(nHash, szCmd, bIsMember);
	}

	BOOL CRedisConnectorPool::SMEMBERS(const char* key, std::vector<std::string>& vctElements, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SMEMBERS %s", key);

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::SMOVE(const char* from, const char* to, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SMOVE %s %s %s", from, to, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SRANDMEMBER(const char* key, int nCount, std::vector<std::string>& vctElements, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SRANDMEMBER %s %d", key, nCount);

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::SREM(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SREM %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SREM(const char* key, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SREM %s %d", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SREM(const char* key, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SREM %s %lld", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::SUNION(const char* key1, const char* key2, std::vector<std::string>& vctElements, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SUNION %s %s", key1, key2);

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::SUNIONSTORE(const char* key1, const char* key2, const char* newkey, int& nNewSize, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "SUNIONSTORE %s %s %s", key1, key2, newkey);

		return ExcuteCommand(nHash, szCmd, nNewSize);
	}

	BOOL CRedisConnectorPool::ZADD(const char* key, int score, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZADD %s %d %s", key, score, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZADD(const char* key, int score, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZADD %s %d %d", key, score, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZADD(const char* key, int score, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZADD %s %d %lld", key, score, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZCARD(const char* key, int& nSize, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZCARD %s", key);

		return ExcuteCommand(nHash, szCmd, nSize);
	}

	BOOL CRedisConnectorPool::ZINCRBY(const char* key, int score, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZINCRBY %s %d %s", key, score, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZINCRBY(const char* key, int score, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZINCRBY %s %d %d", key, score, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZINCRBY(const char* key, int score, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZINCRBY %s %d %lld", key, score, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZRANGE(const char* key, int begin, int end, std::vector<std::string>& vctElements, BOOL bWithScore /* = FALSE */, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZRANGE %s %d %d %s", key, begin, end, (bWithScore ? "WITHSCORES" : ""));

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::ZRANGEBYSCORE(const char* key, int begin_score, int end_score, std::vector<std::string>& vctElements, BOOL bWithScore /* = FALSE */, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZRANGEBYSCORE %s %d %d %s", key, begin_score, end_score, (bWithScore ? "WITHSCORES" : ""));

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::ZRANK(const char* key, const char* value, int& nRank, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZRANK %s %s", key, value);

		return ExcuteCommand(nHash, szCmd, nRank);
	}

	BOOL CRedisConnectorPool::ZRANK(const char* key, int value, int& nRank, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZRANK %s %d", key, value);

		return ExcuteCommand(nHash, szCmd, nRank);
	}

	BOOL CRedisConnectorPool::ZRANK(const char* key, LONGLONG value, int& nRank, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZRANK %s %lld", key, value);

		return ExcuteCommand(nHash, szCmd, nRank);
	}

	BOOL CRedisConnectorPool::ZREM(const char* key, const char* value, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREM %s %s", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZREM(const char* key, int value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREM %s %d", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZREM(const char* key, LONGLONG value, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREM %s %lld", key, value);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZREMRANGEBYRANK(const char* key, int begin, int end, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREMRANGEBYRANK %s %d %d", key, begin, end);

		return ExcuteAsyncCommand(nHash, szCmd);
	}

	BOOL CRedisConnectorPool::ZREVRANGE(const char* key, int begin, int end, std::vector<std::string>& vctElements, BOOL bWithScore /* = FALSE */, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREVRANGE %s %d %d %s", key, begin, end, (bWithScore ? "WITHSCORES" : ""));

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::ZREVRANGEBYSCORE(const char* key, int begin_score, int end_score, std::vector<std::string>& vctElements, BOOL bWithScore /* = FALSE */, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREVRANGEBYSCORE %s %d %d %s", key, begin_score, end_score, (bWithScore ? "WITHSCORES" : ""));

		return ExcuteCommand(nHash, szCmd, vctElements);
	}

	BOOL CRedisConnectorPool::ZREVRANK(const char* key, const char* value, int& nRank, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREVRANK %s %s", key, value);

		return ExcuteCommand(nHash, szCmd, nRank);
	}

	BOOL CRedisConnectorPool::ZREVRANK(const char* key, int value, int& nRank, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREVRANK %s %d", key, value);

		return ExcuteCommand(nHash, szCmd, nRank);
	}

	BOOL CRedisConnectorPool::ZREVRANK(const char* key, LONGLONG value, int& nRank, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZREVRANK %s %lld", key, value);

		return ExcuteCommand(nHash, szCmd, nRank);
	}

	BOOL CRedisConnectorPool::ZSCORE(const char* key, const char* value, int& nScore, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZSCORE %s %s", key, value);

		return ExcuteCommand(nHash, szCmd, nScore);
	}

	BOOL CRedisConnectorPool::ZSCORE(const char* key, int value, int& nScore, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZSCORE %s %d", key, value);

		return ExcuteCommand(nHash, szCmd, nScore);
	}

	BOOL CRedisConnectorPool::ZSCORE(const char* key, LONGLONG value, int& nScore, int nHash /*= 0*/)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZSCORE %s %lld", key, value);

		return ExcuteCommand(nHash, szCmd, nScore);
	}

	BOOL CRedisConnectorPool::ZCOUNT(const char* key, int begin_score, int end_score, int& nCount, int nHash /* = 0 */)
	{
		char szCmd[1024] = { 0 };
		_snprintf_s(szCmd, 1023, "ZCOUNT %s %d %d", key, begin_score, end_score);

		return ExcuteCommand(nHash, szCmd, nCount);
	}
}