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

namespace YTSvrLib
{

	typedef int(*REDIS_HASH_FUNCTION)(int);

	class CRedisConnector
	{
	public:
		CRedisConnector()
		{
			ZeroMemory(m_szIP, sizeof(m_szIP));
			ZeroMemory(m_szAuthPassword, sizeof(m_szAuthPassword));
			m_nPort = 0;
			m_pSyncRedisConnector = NULL;
			m_pAsyncRedisConnector = NULL;

			m_nID = 0;
			m_bInited = FALSE;
			m_bSyncRedisConnected = FALSE;
			m_bAsyncRedisConnected = FALSE;
			m_bAsyncRedisConnecting = FALSE;
		}

		virtual ~CRedisConnector()
		{

		}

		void SetConnectInfo(const char* strIP, int nPort, const char* strAuth);
		void StartRedisConnect();
		BOOL CheckSyncRedisConnect();
		BOOL CheckAsyncRedisConnect();

		void SetID(int nID)
		{
			m_nID = nID;
		}
		int GetID()
		{
			return m_nID;
		}

		BOOL IsYourAsyncObject(const redisAsyncContext* p);

		BOOL ExcuteCommand(const char* pszCmd, char* pszOut, int nMaxLen);
		BOOL ExcuteCommand(const char* pszCmd, std::string& strOut);
		BOOL ExcuteCommand(const char* pszCmd, int& nOut);
		BOOL ExcuteCommand(const char* pszCmd, std::vector<std::string>& vctOuts);
		BOOL ExcuteAsyncCommand(const char* pszCmd);
	public:
		void OnAysncRedisConnected(int status);
		void OnAysncRedisDisconnected(int status);
		void OnAysncRedisAuth(redisReply* reply);
		void OnAysncRedisReply(redisReply* reply);
	protected:
		redisContext* m_pSyncRedisConnector;//异步连接结构
		redisAsyncContext* m_pAsyncRedisConnector;//同步连接结构

		int m_nID;
		char m_szIP[64];
		char m_szAuthPassword[128];
		int m_nPort;
		BOOL m_bInited;
		BOOL m_bSyncRedisConnected;
		BOOL m_bAsyncRedisConnected;
		BOOL m_bAsyncRedisConnecting;
	};

	class CRedisConnectorPool : public CSingle < CRedisConnectorPool >
	{
	public:
		CRedisConnectorPool()
		{
			m_pAsyncRedisEventBase = NULL;
			m_pHashFunction = NULL;
			m_vctRedisPool.clear();
		}

		virtual ~CRedisConnectorPool()
		{

		}

		// hiredis异步回调
		static void OnAsyncRedisConnected(const redisAsyncContext *c, int status);
		static void OnAsyncRedisDisconnected(const redisAsyncContext *c, int status);
		static void OnAsyncRedisAuthReply(redisAsyncContext *, void *reply, void *privdata);//这个专门用来处理AUTH的回调
		static void OnAsyncRedisReply(redisAsyncContext *, void *reply, void *privdata);
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
		// 获取libevent线程的内核对象
		event_base* GetEventBase()
		{
			return m_pAsyncRedisEventBase;
		}
		// 通过id来获取一个连接器.
		CRedisConnector* GetRedisConnector(int nID);
		// 通过redisAsyncContext*获取一个连接器.便于在redis回调时找到连接器
		CRedisConnector* GetRedisConnector(const redisAsyncContext* p);
	public:
		// 执行一个命令.并返回一个c风格字符串.注意:当返回值不是字符串/状态类型时.调用会失败.返回值的类型请参考redis官网
		BOOL ExcuteCommand(int nHash, const char* pszCmd, char* pszOut, int nMaxLen);
		// 执行一个命令.并返回一个stl风格字符串.注意:当返回值不是字符串/状态类型时.调用会失败.返回值的类型请参考redis官网
		BOOL ExcuteCommand(int nHash, const char* pszCmd, std::string& strOut);
		// 执行一个命令.并返回一个数字.注意:当返回值不是int类型时.调用会失败.返回值的类型请参考redis官网
		BOOL ExcuteCommand(int nHash, const char* pszCmd, int& nOut);
		// 执行一个命令.并返回一个数组.注意:当返回值不是array类型时.调用会失败.返回值的类型请参考redis官网
		BOOL ExcuteCommand(int nHash, const char* pszCmd, std::vector<std::string>& vctOuts);
		// 异步执行一个命令.
		BOOL ExcuteAsyncCommand(int nHash, const char* pszCmd);
	public:
		// 删除一个key
		BOOL DEL(const char* key, int nHash = 0);
		// 通知redis后台保存数据到硬盘
		BOOL BGSAVE(int nHash = 0);
		// 通知redis后台保存AOF文件
		BOOL BGREWRITEAOF(int nHash = 0);
		// 向一个key中追加value内容
		BOOL APPEND(const char* key, const char* value, int nHash = 0);
		// 将一个key中的数值减去nDec大小.并返回减去后的新值保存在nNew中
		BOOL DECRBY(const char* key, int nDec, int& nNew, int nHash = 0);
		// 将一个key中的数值增加nInc大小.并返回增加后的新值保存在nNew中
		BOOL INCRBY(const char* key, int nInc, int& nNew, int nHash = 0);
		// 设置某个key的在tSec后自动过期
		BOOL EXPIRE(const char* key, __time32_t tSec, int nHash = 0);
		// 设置某个key的在时间到达tExpired时自动过期
		BOOL EXPIREAT(const char* key, __time32_t tExpired, int nHash = 0);
		// 设置一个key=>value值
		BOOL SET(const char* key, const char* value, int nHash = 0);
		BOOL SET(const char* key, int value, int nHash = 0);
		BOOL SET(const char* key, LONGLONG value, int nHash = 0);
		// 新增一个key=>value值.当key已存在则失败
		BOOL SETNX(const char* key, const char* value, int nHash = 0);
		BOOL SETNX(const char* key, int value, int nHash = 0);
		BOOL SETNX(const char* key, LONGLONG value, int nHash = 0);
		// 获取一个key的value值
		BOOL GET(const char* key, std::string& strValue, int nHash = 0);
		BOOL GET(const char* key, int& nValue, int nHash = 0);
		BOOL GET(const char* key, LONGLONG& nValue, int nHash = 0);
		// 批量获取
		BOOL MGET(const std::vector<std::string>& vctKeys, std::vector<std::string>& vctValues, int nHash = 0);
		BOOL EXISTS(const char* key, BOOL& bExist, int nHash = 0);
		BOOL TTL(const char* key, __time32_t& tSec, int nHash = 0);
		BOOL KEYS(const char* pattern, std::vector<std::string>& vctKeys, int nHash = 0);
		BOOL RENAME(const char* key, const char* newkey, int nHash = 0);
		BOOL RENAMENX(const char* key, const char* newkey, int nHash = 0);
		BOOL STRLEN(const char* key, int& nSize, int nHash = 0);

		BOOL HDEL(const char* key, const char* field, int nHash = 0);
		BOOL HEXISTS(const char* key, const char* field, BOOL& bExist, int nHash = 0);
		BOOL HGET(const char* key, const char* field, std::string& strValues, int nHash = 0);
		BOOL HGET(const char* key, const char* field, int& nValue, int nHash = 0);
		BOOL HGET(const char* key, const char* field, LONGLONG& nValue, int nHash = 0);
		BOOL HGET(const char* key, int field, std::string& strValues, int nHash = 0);
		BOOL HGET(const char* key, int field, int& nValue, int nHash = 0);
		BOOL HGET(const char* key, int field, LONGLONG& nValue, int nHash = 0);
		BOOL HGETALL(const char* key, std::map<std::string, std::string>& mapValues, int nHash = 0);
		BOOL HINCRBY(const char* key, const char* field, int nInc, int& nNew, int nHash = 0);
		BOOL HKEYS(const char* key, std::vector<std::string>& vctFields, int nHash = 0);
		BOOL HLEN(const char* key, int& nLen, int nHash = 0);
		BOOL HMGET(const char* key, const std::vector<std::string>& vctFileds, std::vector<std::string>& vctValues, int nHash = 0);
		BOOL HMSET(const char* key, const std::vector<std::string>& vctFileds, const std::vector<std::string>& vctValues, int nHash = 0);
		BOOL HMSET(const char* key, const std::vector<int>& vctFileds, const std::vector<std::string>& vctValues, int nHash = 0);
		BOOL HSET(const char* key, const char* field, const char* value, int nHash = 0);
		BOOL HSET(const char* key, const char* field, int value, int nHash = 0);
		BOOL HSET(const char* key, const char* field, LONGLONG value, int nHash = 0);
		BOOL HSET(const char* key, int field, const char* value, int nHash = 0);
		BOOL HSET(const char* key, int field, int value, int nHash = 0);
		BOOL HSET(const char* key, int field, LONGLONG value, int nHash = 0);
		BOOL HSETNX(const char* key, const char* field, const char* value, int nHash = 0);
		BOOL HSETNX(const char* key, const char* field, int value, int nHash = 0);
		BOOL HSETNX(const char* key, const char* field, LONGLONG value, int nHash = 0);
		BOOL HSTRLEN(const char* key, const char* field, int& nSize, int nHash = 0);

		BOOL LINDEX(const char* key, int nIndex, std::string& strValue, int nHash = 0);
		BOOL LINSERT(const char* key, const char* find, const char* newvalue, int nHash = 0);
		BOOL LINSERT(const char* key, const char* find, int newvalue, int nHash = 0);
		BOOL LINSERT(const char* key, const char* find, LONGLONG newvalue, int nHash = 0);
		BOOL LLEN(const char* key, int& nSize, int nHash = 0);
		BOOL LPOP(const char* key, std::string& strValue, int nHash = 0);
		BOOL LPOP(const char* key, int& nValue, int nHash = 0);
		BOOL LPOP(const char* key, LONGLONG& nValue, int nHash = 0);
		BOOL LPUSH(const char* key, const char* value, int nHash = 0);
		BOOL LPUSH(const char* key, int value, int nHash = 0);
		BOOL LPUSH(const char* key, LONGLONG value, int nHash = 0);
		BOOL LPUSHX(const char* key, const char* value, int nHash = 0);
		BOOL LRANGE(const char* key, int begin, int end, std::vector<std::string>& vctValues, int nHash = 0);
		BOOL LREM(const char* key, int count_to_remove, const char* find, int nHash = 0);
		BOOL LSET(const char* key, int index, const char* newvalue, int nHash = 0);
		BOOL LSET(const char* key, int index, int newvalue, int nHash = 0);
		BOOL LSET(const char* key, int index, LONGLONG newvalue, int nHash = 0);
		BOOL RPOP(const char* key, std::string& strValue, int nHash = 0);
		BOOL RPOP(const char* key, int& nValue, int nHash = 0);
		BOOL RPOP(const char* key, LONGLONG& nValue, int nHash = 0);
		BOOL RPOPLPUSH(const char* key, const char* dest, std::string& strValue, int nHash = 0);
		BOOL RPUSH(const char* key, const char* value, int nHash = 0);
		BOOL RPUSH(const char* key, int value, int nHash = 0);
		BOOL RPUSH(const char* key, LONGLONG value, int nHash = 0);
		BOOL RPUSHX(const char* key, const char* value, int nHash = 0);

		BOOL SADD(const char* key, const char* value, int nHash = 0);
		BOOL SADD(const char* key, int value, int nHash = 0);
		BOOL SADD(const char* key, LONGLONG value, int nHash = 0);
		BOOL SCARD(const char* key, int& nSize, int nHash = 0);
		BOOL SDIFF(const char* key1, const char* key2, std::vector<std::string>& vctElements, int nHash = 0);
		BOOL SDIFFSTORE(const char* key1, const char* key2, const char* newkey, int& nNewSize, int nHash = 0);
		BOOL SINTER(const char* key1, const char* key2, std::vector<std::string>& vctElements, int nHash = 0);
		BOOL SINTERSTORE(const char* key1, const char* key2, const char* newkey, int& nNewSize, int nHash = 0);
		BOOL SISMEMBER(const char* key, const char* value, BOOL& bIsMember, int nHash = 0);
		BOOL SISMEMBER(const char* key, int value, BOOL& bIsMember, int nHash = 0);
		BOOL SISMEMBER(const char* key, LONGLONG value, BOOL& bIsMember, int nHash = 0);
		BOOL SMEMBERS(const char* key, std::vector<std::string>& vctElements, int nHash = 0);
		BOOL SMOVE(const char* from, const char* to, const char* value, int nHash = 0);
		BOOL SRANDMEMBER(const char* key, int nCount, std::vector<std::string>& vctElements, int nHash = 0);
		BOOL SREM(const char* key, const char* value, int nHash = 0);
		BOOL SREM(const char* key, int value, int nHash = 0);
		BOOL SREM(const char* key, LONGLONG value, int nHash = 0);
		BOOL SUNION(const char* key1, const char* key2, std::vector<std::string>& vctElements, int nHash = 0);
		BOOL SUNIONSTORE(const char* key1, const char* key2, const char* newkey, int& nNewSize, int nHash = 0);

		BOOL ZADD(const char* key, int score, const char* value, int nHash = 0);
		BOOL ZADD(const char* key, int score, int value, int nHash = 0);
		BOOL ZADD(const char* key, int score, LONGLONG value, int nHash = 0);
		BOOL ZCARD(const char* key, int& nSize, int nHash = 0);
		BOOL ZINCRBY(const char* key, int score, const char* value, int nHash = 0);
		BOOL ZINCRBY(const char* key, int score, int value, int nHash = 0);
		BOOL ZINCRBY(const char* key, int score, LONGLONG value, int nHash = 0);
		BOOL ZRANGE(const char* key, int begin, int end, std::vector<std::string>& vctElements, BOOL bWithScore = FALSE, int nHash = 0);
		BOOL ZRANGEBYSCORE(const char* key, int begin_score, int end_score, std::vector<std::string>& vctElements, BOOL bWithScore = FALSE, int nHash = 0);
		BOOL ZRANK(const char* key, const char* value, int& nRank, int nHash = 0);
		BOOL ZRANK(const char* key, int value, int& nRank, int nHash = 0);
		BOOL ZRANK(const char* key, LONGLONG value, int& nRank, int nHash = 0);
		BOOL ZREM(const char* key, const char* value, int nHash = 0);
		BOOL ZREM(const char* key, int value, int nHash = 0);
		BOOL ZREM(const char* key, LONGLONG value, int nHash = 0);
		BOOL ZREMRANGEBYRANK(const char* key, int begin, int end, int nHash = 0);
		BOOL ZREVRANGE(const char* key, int begin, int end, std::vector<std::string>& vctElements, BOOL bWithScore = FALSE, int nHash = 0);
		BOOL ZREVRANGEBYSCORE(const char* key, int begin_score, int end_score, std::vector<std::string>& vctElements, BOOL bWithScore = FALSE, int nHash = 0);
		BOOL ZREVRANK(const char* key, const char* value, int& nRank, int nHash = 0);
		BOOL ZREVRANK(const char* key, int value, int& nRank, int nHash = 0);
		BOOL ZREVRANK(const char* key, LONGLONG value, int& nRank, int nHash = 0);
		BOOL ZSCORE(const char* key, const char* value, int& nScore, int nHash = 0);
		BOOL ZSCORE(const char* key, int value, int& nScore, int nHash = 0);
		BOOL ZSCORE(const char* key, LONGLONG value, int& nScore, int nHash = 0);
		BOOL ZCOUNT(const char* key, int begin_score, int end_score, int& nCount, int nHash = 0);
	public:
		static const char* MakeKey(const char* fmt, ...);
	protected:
		static char m_szPublicKeyTemp[1024 * 8];
#ifdef LIB_WINDOWS
		// 异步回调线程
		static unsigned WINAPI ThreadRedisAsync(LPVOID pParam);
#else
		// 异步回调线程
		static void* ThreadRedisAsync(LPVOID pParam);
#endif // LIB_WINDOWS

#ifdef LIB_WINDOWS
		HANDLE m_hThread;
#else
		pthread_t m_hThread;
#endif // LIB_WINDOWS
		event_base* m_pAsyncRedisEventBase;//异步调用依赖的事件
		vector<CRedisConnector*> m_vctRedisPool;
		REDIS_HASH_FUNCTION m_pHashFunction;
	};
}

#endif // !__REDIS_CONNECTOR_H_
