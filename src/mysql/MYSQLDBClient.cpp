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
#include "MYSQLDBSys.h"
#include "MYSQLManagerBase.h"
#include "mysqlpp/dbdriver.h"

namespace YTSvrLib
{

	CMYSQLClient::CMYSQLClient(CMYSQLDBSystem *pSystem, BOOL bThreadSafe) : m_DBSys(false), m_ReqObjPool("MYSQLSTRING_REQUEST")
	{
		m_pDBSystem = pSystem;
		m_hSemaphore.Create(NULL, 0, 0xFFFFFF);
		//m_ReqObjPool.Increase(nMaxSize);
		//m_nID = (UINT)this;
		m_bNeedReset = FALSE;
		m_bThreadSafe = bThreadSafe;
	}

	void CMYSQLClient::SetConnectionInfo(MYSQLCONNECT_INFO& sConnect)
	{
		memcpy(&m_ConnectInfo, &sConnect, sizeof(m_ConnectInfo));
	}

	BOOL CMYSQLClient::ConnectDB(void)
	{
		if (m_DBSys.connected())
		{
			return TRUE;
		}
		m_DBSys.set_option(new mysqlpp::MultiResultsOption(true));
		m_DBSys.set_option(new mysqlpp::ReconnectOption(true));
		m_DBSys.set_option(new mysqlpp::SetCharsetNameOption("utf8mb4"));
		if (m_ConnectInfo.m_nReadTimeout > 0)
		{
			m_DBSys.set_option(new mysqlpp::ReadTimeoutOption(m_ConnectInfo.m_nReadTimeout));
		}
		if (m_ConnectInfo.m_nWriteTimeout > 0)
		{
			m_DBSys.set_option(new mysqlpp::WriteTimeoutOption(m_ConnectInfo.m_nWriteTimeout));
		}

		LOG("Connecting MySQL Database : Host=%s:%d DB=%s", m_ConnectInfo.m_szHostname, m_ConnectInfo.m_nPort, m_ConnectInfo.m_szDefaultDB);
		if (m_DBSys.connect(
			m_ConnectInfo.m_szDefaultDB,
			m_ConnectInfo.m_szHostname,
			m_ConnectInfo.m_szDBUser,
			m_ConnectInfo.m_szDBPass,
			m_ConnectInfo.m_nPort) == false)
		{
			LOGERROR("Database Real Connect Error=%s", m_DBSys.error());

			return FALSE;
		}

		return TRUE;
	}

	BOOL CMYSQLClient::ReconnectDB()
	{
		if (m_DBSys.connected())
		{
			m_DBSys.disconnect();
		}

		return ConnectDB();
	}

	bool CMYSQLClient::PreLoopFunc()
	{
		if (FALSE == ConnectDB())
		{
			LOG("Connect DB error!");
			return false;
		}

		LOG("Connect DB success!");
		return true;
	}

	void CMYSQLClient::Reset()
	{
		m_PoolLock.Lock();
		m_bNeedReset = TRUE;
		m_PoolLock.UnLock();
	}

	void CMYSQLClient::OnTerminated(void)
	{
		m_DBSys.disconnect();
	}

	BOOL CMYSQLClient::RecvSQLRequest(MYSQLSTRING_REQUEST* pRequest)
	{
		m_ReqQueue.Lock();
		m_ReqQueue.push_back(pRequest);
#ifdef COREDEBUG
		LOG("RecvSQLRequest m_ReqQueue push_back = %d", m_ReqQueue.size());
#endif
		m_ReqQueue.UnLock();

		m_hSemaphore.UnLock(1);
		return  TRUE;
	}

	void CMYSQLClient::OnError(int nErrorCode)
	{
		if (nErrorCode)
		{
#ifdef COREDEBUG
			LOG("CMYSQLClient::OnError:Code = %d", nErrorCode);
#endif
		}
	}

	BOOL CMYSQLClient::ExecuteSQL(MYSQLSTRING_REQUEST *pSqlStr)
	{
		MYSQLLIB::CMYSQLRecordSet * pResSet = m_pDBSystem->ApplyRecSet();
		if (pResSet == NULL)
		{
			return FALSE;
		}

		CMYSQLQueryInfo* pQueryInfo = (CMYSQLQueryInfo*) pSqlStr->m_nKey;

		m_dwExecuteCount++;
		std::string strSQL = pSqlStr->m_pCmd->GetSQLRequest();

		ULONG nAffectRows = 0;
		try
		{
			if (m_bThreadSafe)
			{
				m_RequestLock.Lock();
			}

#ifdef COREDEBUG
			LOG("CMYSQLClient::ExecuteSQL(MYSQLSTRING_REQUEST *pSqlStr) BEGIN");
#endif
			DWORD dwTick = GetTickCount();
			mysqlpp::Query CurQuery = m_DBSys.query(strSQL);
			DWORD dwTick2 = GetTickCount();
#ifdef COREDEBUG
			LOG("CMYSQLClient::ExecuteSQL(MYSQLSTRING_REQUEST *pSqlStr) query End");
#endif
			DWORD dwCountInQueue = GetReqCountInQueue();
			if (pQueryInfo && (dwTick2 >= dwTick + 200 || dwCountInQueue > 500))
			{
				LOGTRACE("DBClient=%d ExecuteSQL=%s pResSet=0x%08x Dur=%d Queue=%d", GetID(), pQueryInfo->m_strSPName, pResSet, dwTick2 - dwTick, dwCountInQueue);
			}

			BOOL bExcute = pSqlStr->m_pCmd->IsExcute();

			CurQuery.enable_exceptions();

			if (bExcute)
			{
#ifdef COREDEBUG
				LOG("CMYSQLClient::ExecuteSQL(MYSQLSTRING_REQUEST *pSqlStr) execute begin");
#endif
				mysqlpp::SimpleResult Res = CurQuery.execute();
#ifdef COREDEBUG
				LOG("CMYSQLClient::ExecuteSQL(MYSQLSTRING_REQUEST *pSqlStr) execute end");
#endif
			}
			else
			{
				mysqlpp::StoreQueryResult Res = CurQuery.store();
				if (Res)
				{
#ifdef COREDEBUG
					LOG("CMYSQLClient::ExecuteSQL(MYSQLSTRING_REQUEST *pSqlStr) AddResult begin");
#endif
					do
					{
						pResSet->AddResult(std::move(Res));

						Res = CurQuery.store_next();
					} while (CurQuery.more_results());
#ifdef COREDEBUG
					LOG("CMYSQLClient::ExecuteSQL(MYSQLSTRING_REQUEST *pSqlStr) AddResult end");
#endif
				}
			}

			nAffectRows = (ULONG) CurQuery.affected_rows();

			if (m_bThreadSafe)
			{
				m_RequestLock.UnLock();
			}
		}
		catch (mysqlpp::BadQuery& err)
		{
			if (m_bThreadSafe)
			{
				m_RequestLock.UnLock();
			}

			int nErrorCode = err.errnum();

			if (pSqlStr->m_nTry >= m_pDBSystem->GetRetryCount())
			{
				if (pQueryInfo)
				{
					LOGWARN("DBClient=%d ExecuteSQL=%s pResSet=0x%08x ErrorCode=%d Error=%s Queue=%d", GetID(), pQueryInfo->m_strSPName, pResSet, nErrorCode, err.what(), GetReqCountInQueue());
				}

				if (pSqlStr->m_pProcFun)
				{
					pSqlStr->m_pProcFun(nErrorCode, 0, pQueryInfo, NULL, pSqlStr->m_bIsAsync);
				}
				else
				{
					LOG("Invalid Proc Function : NULL : QueryInfo : 0x%x", pQueryInfo);
				}
			}
			else
			{
				if (pQueryInfo)
				{
					LOG("DBClient=%d ExecuteSQL=%s pResSet=0x%08x ErrorCode=%d Error=%s Queue=%d", GetID(), pQueryInfo->m_strSPName, pResSet, nErrorCode, err.what(), GetReqCountInQueue());
				}
			}

			OnError(nErrorCode);

			m_pDBSystem->ReclaimRecSet(pResSet);

			return FALSE;
		}

		if (pSqlStr->m_pProcFun)
		{
			pSqlStr->m_pProcFun(0, nAffectRows, pQueryInfo, pResSet, pSqlStr->m_bIsAsync);
		}
		else
		{
			LOG("Invalid Proc Function : NULL : QueryInfo : 0x%x", pQueryInfo);
		}

		return TRUE;
	}

	void CMYSQLClient::Execute(void)
	{
		MYSQLSTRING_REQUEST * pSqlReq = NULL;
		BOOL  bResCode = FALSE;

		while (!m_bTerminated)
		{
			if (m_bNeedReset)
			{
				LOG("DBClient=%d Reseting", GetID());
				ReconnectDB();
				m_PoolLock.Lock();
				m_bNeedReset = FALSE;
				m_PoolLock.UnLock();
			}
			pSqlReq = NULL;
			bResCode = m_hSemaphore.Lock(2000);
			if (!bResCode)
			{
#ifdef COREDEBUG
				LOG("CMYSQLClient::Execute(void) = !bResCode");
#endif // COREDEBUG
				continue;
			}

			m_ReqQueue.Lock();
			if (m_ReqQueue.empty())
			{
#ifdef COREDEBUG
				LOG("CMYSQLClient::Execute(void) = m_ReqQueue.empty()");
#endif // COREDEBUG
				m_ReqQueue.UnLock();
				continue;
			}
			pSqlReq = m_ReqQueue.pop_front();
			m_ReqQueue.UnLock();

			if (pSqlReq != NULL)
			{
				BOOL bRet = FALSE;
				//TRY_BEGIN_EXCEP
				bRet = ExecuteSQL(pSqlReq);
				//TRY_END_EXCEP("CMYSQLClient::ExecuteSQL()");
				if (bRet == FALSE && pSqlReq->m_nTry < m_pDBSystem->GetRetryCount())
				{
					pSqlReq->m_nTry++;
					LOG("Retry DBSQL Query : pSqlReq=0x%08x pCmd=0x%08x nCurRetry=%d", pSqlReq, pSqlReq->m_pCmd, pSqlReq->m_nTry);
					m_ReqQueue.Lock();
					m_ReqQueue.push_front(pSqlReq);
					m_ReqQueue.UnLock();
					m_hSemaphore.UnLock(1);
				}
				else
				{
					if (pSqlReq->m_pCmd)
						m_pDBSystem->ReclaimCmd(pSqlReq->m_pCmd);
					ReclaimSQLReq(pSqlReq);
				}
			}
		}
	}

	BOOL CMYSQLClient::RecvSyncSQLRequest(MYSQLSTRING_REQUEST* pSqlReq)
	{
		BOOL bRet = FALSE;
		//TRY_BEGIN_EXCEP
		bRet = ExecuteSQL(pSqlReq);
		//TRY_END_EXCEP("CMYSQLClient::RecvSyncSQLRequest()");

		if (pSqlReq->m_pCmd)
			m_pDBSystem->ReclaimCmd(pSqlReq->m_pCmd);
		ReclaimSQLReq(pSqlReq);

		return bRet;
	}

	void CMYSQLClient::Ping()
	{
		m_DBSys.ping();
	}

	UINT CMYSQLClient::EscapeString(std::string* out, const char* src, size_t len)
	{
		if (m_DBSys.connected() == false)
		{
			return 0;
		}

		mysqlpp::DBDriver* pCore = m_DBSys.driver();

		if (pCore)
		{
			return (UINT) pCore->escape_string(out, src, len);
		}

		return 0;
	}
}