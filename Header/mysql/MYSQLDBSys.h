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

#ifndef _MYSQLDBSYS_H_
#define _MYSQLDBSYS_H_
#pragma warning(disable:4005)
#include "mysqlpp/mysql++.h"
#include "MYSQLCommand.h"
#include "MYSQLRecordSet.h"
#include <vector>

#define INVALID_DBQUERY_KEY -1

namespace YTSvrLib
{
	typedef DWORD(*MYSQLProcFunc)(UINT nErrorCode, ULONG nEffectRows, void* nKey, YTSvrLib::MYSQLLIB::CMYSQLRecordSet * pResSet, BOOL bAsync);

	typedef struct YTSVRLIB_EXPORT tagMYSQLConnectInfo
	{
		int  m_nSN;                   //DB服务器编号（0~n）
		char m_szHostname[128];
		char m_szDBUser[32];
		char m_szDBPass[32];
		char m_szDefaultDB[128];
		UINT m_nPort;
		int m_nReadTimeout;
		int m_nWriteTimeout;
	}MYSQLCONNECT_INFO, *PMYSQLCONNECT_INFO;

	class CMYSQLClient;
	class YTSVRLIB_EXPORT CMYSQLDBSystem : public CThreadPool
	{
		friend  class  CMYSQLClient;
	public:

		CMYSQLDBSystem(void) :m_RecSetPool("CMYSQLRecordSet"), m_CommandPool("CMYSQLCommand")
		{
			m_nDbCount = 1;
			m_nHashNum = 1;
			m_tLastSQLRequest = 0;
			m_nRetryCount = 0;
		}
		virtual ~CMYSQLDBSystem(void)
		{}

		///增加连接mysql server信息
		void AddServerConnectInfo(LPCSTR lpszHost, LPCSTR lpszDBUser, LPCSTR lpszDBPass, LPCSTR lpszDefaultDB, UINT nPort, int nReadTimeout = 0, int nWriteTimeout = 0);
		///系统启动	
		// nRetryCount = 失败的命令重试的次数
		// bThreadSafe = 是否要在执行数据库命令时对MYSQL内核对象加锁.(在同时允许同步异步操作的服务器上必须要加锁.否则会在异步操作时同步的线程也进入引发Crush)
		BOOL   Startup(UINT nRetryCount = 0, BOOL bThreadSafe = FALSE);
		///获取启动失败的连接vector
		std::vector<int>  GetFailedConnVec(void)
		{
			return m_vecFailedConnDB;
		}
		///增加SQL操作
		BOOL AddSQLRequest(void* dwKey, long nHashParm, CMYSQLCommand* pCmd, MYSQLProcFunc pFunc);
		//同步SQL操作
		BOOL AddSyncSQLRequest(void* dwKey, long nHashParm, CMYSQLCommand* pCmd, MYSQLProcFunc pFunc);
		///获取哈希客户端
		CMYSQLClient* GetClientHash(long nHashParm);

		BOOL	IsAllDBClientIdle();
		size_t	GetAllCmdInQueue();
		UINT	GetRetryCount() const
		{
			return m_nRetryCount;
		}

		BOOL	Lock();
		void	Unlock();

		UINT EscapeString(char* out, const char* src, size_t len);
	public:
		MYSQLLIB::CMYSQLRecordSet* ApplyRecSet()
		{
			return m_RecSetPool.ApplyObj();
		}
		void ReclaimRecSet(MYSQLLIB::CMYSQLRecordSet* pRecSet)
		{
			pRecSet->Close();
			m_RecSetPool.ReclaimObj(pRecSet);
		}

		CMYSQLCommand* ApplyCmd()
		{
			CMYSQLCommand* pCmd = m_CommandPool.ApplyObj();
			return pCmd;
		}
		void ReclaimCmd(CMYSQLCommand* pCmd)
		{
			m_CommandPool.ReclaimObj(pCmd);
		}
	protected:
		virtual size_t HashAlloteDBServer(size_t nHashParm)
		{
			if (m_nDbCount <= 0)
				return 0;
			size_t nRet = nHashParm % m_nDbCount;
			return nRet;
		}
	protected:
		std::vector< MYSQLCONNECT_INFO  >	m_vecConnectInfo;
		std::vector< int >					m_vecFailedConnDB;
		int									m_nDbCount;
		DWORD								m_nHashNum;
		UINT								m_nRetryCount;
		YTSvrLib::CLock						m_InitLock;
		CPool<MYSQLLIB::CMYSQLRecordSet, 512>			m_RecSetPool;
		CPool<CMYSQLCommand, 1024>			m_CommandPool;

		__time32_t							m_tLastSQLRequest;
	};

	class YTSVRLIB_EXPORT CMYSQLClient : public CThread
	{
		friend   class  CMYSQLDBSystem;

	public:

#define  DEFAULTPOOLSIZE  1000
#define	MAX_DATABASE_TRY	3
		typedef  struct  tagMYSQLStringRequest :public CRecycle //SQL语句请求信息   
		{
			void*			m_nKey;
			unsigned long	m_nHash;
			CMYSQLCommand*	m_pCmd;
			BOOL			m_bIsAsync;//是否是异步的
			MYSQLProcFunc	m_pProcFun;
			UINT			m_nTry;

			tagMYSQLStringRequest()
			{
				Init();
			}

			virtual void Init()
			{
				m_nKey = NULL;
				m_pCmd = 0;
				m_nHash = 0;
				m_pProcFun = NULL;
				m_nTry = 0;
				m_bIsAsync = false;
			}
			virtual void Clear()
			{}
		}MYSQLSTRING_REQUEST;
		typedef CWQueue<MYSQLSTRING_REQUEST*> QueueMYSQLReq;

	public:

		CMYSQLClient(CMYSQLDBSystem *pSystem, BOOL bThreadSafe);
		virtual ~CMYSQLClient(void)
		{
			Clear();
		}
		///连接MYSQL DB
		BOOL	ConnectDB(void);
		BOOL	ReconnectDB();
		BOOL	IsConnected()
		{
			return (BOOL) m_DBSys.connected();
		}
		void	OnError(int nErrorCode);

		virtual void Init()
		{}
		virtual void Clear()
		{}

		UINT EscapeString(char* out, const char* src, size_t len);

		void SetConnectionInfo(MYSQLCONNECT_INFO& sConnect);

		MYSQLSTRING_REQUEST* ApplySQLReq()
		{
			return m_ReqObjPool.ApplyObj();
		}
		void ReclaimSQLReq(MYSQLSTRING_REQUEST* pReq)
		{
			m_ReqObjPool.ReclaimObj(pReq);
		}
		UINT GetReqCountInQueue()
		{
			m_ReqQueue.Lock();
			UINT nSize = m_ReqQueue.size();
			m_ReqQueue.UnLock();
			return nSize;
		}
		DWORD GetExecuteCount()
		{
			return m_dwExecuteCount;
		}
		void	SetID(size_t n)
		{
			m_nID = n;
		}
		size_t	GetID()
		{
			return m_nID;
		}
		void	Reset();
		///Ping服务器
		void Ping();
	private:
		///线程操作前准备
		virtual bool PreLoopFunc();
		//线程结束
		virtual void OnTerminated(void);
		///接受异步SQL请求
		BOOL   RecvSQLRequest(MYSQLSTRING_REQUEST* pRequest);
		///接受同步SQL请求
		BOOL   RecvSyncSQLRequest(MYSQLSTRING_REQUEST* pSqlReq);
		///执行SQL语句请求
		BOOL   ExecuteSQL(MYSQLSTRING_REQUEST *pSqlStr);
		///线程主体
		virtual void Execute(void);
	private:
		BOOL m_bThreadSafe{FALSE};
		YTSvrLib::CLock m_RequestLock;
		MYSQLCONNECT_INFO m_ConnectInfo;
		CMYSQLDBSystem* m_pDBSystem;
		///SQL语句请求列表 
		QueueMYSQLReq m_ReqQueue;
		///SQL请求对象分配池
		CPool<MYSQLSTRING_REQUEST, 1024> m_ReqObjPool;
		///信号量句柄   
		YTSvrLib::CSemaphore m_hSemaphore;
		YTSvrLib::CLock m_PoolLock;
		mysqlpp::Connection m_DBSys;
		DWORD	m_dwExecuteCount{0};
		size_t	m_nID{0};
		BOOL	m_bNeedReset{FALSE};
	};
}

#endif