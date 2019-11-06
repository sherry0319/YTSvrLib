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

namespace YTSvrLib
{

	void CMYSQLDBSystem::AddServerConnectInfo(LPCSTR lpszHost, LPCSTR lpszDBUser, LPCSTR lpszDBPass, LPCSTR lpszDefaultDB, UINT nPort, int nReadTimeout, int nWriteTimeout)
	{
		MYSQLCONNECT_INFO sConnect;

		sConnect.m_nSN = m_nDbCount++;
		sConnect.m_nPort = nPort;
		sConnect.m_nReadTimeout = nReadTimeout;
		sConnect.m_nWriteTimeout = nWriteTimeout;
		strncpy_s(sConnect.m_szHostname, lpszHost, 127);
		strncpy_s(sConnect.m_szDBUser, lpszDBUser, 31);
		strncpy_s(sConnect.m_szDBPass, lpszDBPass, 31);
		strncpy_s(sConnect.m_szDefaultDB, lpszDefaultDB, 127);

		m_vecConnectInfo.push_back(sConnect);
	}

	BOOL CMYSQLDBSystem::IsAllDBClientIdle()
	{
		size_t nClients = GetThreadCount();
		BOOL bAllIdle = TRUE;
		for (size_t i = 0; i < nClients; i++)
		{
			CMYSQLClient  *pMYSQLClient = (CMYSQLClient  *) GetThread(i);
			if (pMYSQLClient)
			{
				UINT nReqLeft = pMYSQLClient->GetReqCountInQueue();
				if (nReqLeft > 0)
				{
					LOG("DBClient_%d Req Left=%d Total=%d", pMYSQLClient->GetID(), nReqLeft, pMYSQLClient->GetExecuteCount());
					bAllIdle = FALSE;
				}
				else
					LOG("DBClient_%d Queue Empty.", pMYSQLClient->GetID());
			}
		}
		return bAllIdle;
	}

	size_t	CMYSQLDBSystem::GetAllCmdInQueue()
	{
		size_t nClients = GetThreadCount();
		size_t nCount = 0;
		for (size_t i = 0; i < nClients; i++)
		{
			CMYSQLClient  *pMYSQLClient = (CMYSQLClient  *) GetThread(i);
			if (pMYSQLClient)
			{
				nCount += (size_t) pMYSQLClient->GetReqCountInQueue();
			}
		}
		return nCount;
	}

	BOOL CMYSQLDBSystem::Startup(UINT nRetryCount, BOOL bThreadSafe /*= FALSE*/)
	{
		CMYSQLClient  *pDBClient = NULL;
		BOOL  bSuc = TRUE;
		m_nRetryCount = nRetryCount;
		m_vecFailedConnDB.clear();

		for (size_t i = 0; i < m_vecConnectInfo.size(); ++i)
		{
			pDBClient = new CMYSQLClient(this, bThreadSafe);

			pDBClient->SetID(i);
			pDBClient->SetConnectionInfo(m_vecConnectInfo[i]);

			AddThread(pDBClient);
		}
		m_nDbCount = (int) GetThreadCount();
		//Ïß³Ì³ØÆô¶¯
		Start(500);
		return bSuc;
	}

	BOOL CMYSQLDBSystem::Lock()
	{
		m_InitLock.Lock();

		return TRUE;
	}

	UINT CMYSQLDBSystem::EscapeString(std::string* out, const char* src, size_t len)
	{
		CMYSQLClient* pClient = GetClientHash((long) len);

		if (pClient)
		{
			return pClient->EscapeString(out, src, len);
		}

		return 0;
	}

	void CMYSQLDBSystem::Unlock()
	{
		m_InitLock.UnLock();
	}

	CMYSQLClient* CMYSQLDBSystem::GetClientHash(long nHashParm)
	{
		size_t nCursor = HashAlloteDBServer(nHashParm);
		if (nCursor >= (int) GetThreadCount())
		{
			LOG("CMYSQLDBSystem_AddSQLRequest HashAlloteDBServer=%d Failed!", nCursor);
			return  NULL;
		}

		CMYSQLClient  *pMYSQLClient = NULL;
		pMYSQLClient = (CMYSQLClient  *) GetThread(nCursor);
		size_t nCursor0 = nCursor;
		while (pMYSQLClient && pMYSQLClient->IsConnected() == FALSE)
		{
			nCursor++;
			if (nCursor >= GetThreadCount())
				nCursor = 0;
			pMYSQLClient = (CMYSQLClient*) GetThread(nCursor);
			if (nCursor == nCursor0)
				break;
		}

		return pMYSQLClient;
	}

	BOOL CMYSQLDBSystem::AddSQLRequest(void* dwKey, long nHashParm, CMYSQLCommand* pCmd, MYSQLProcFunc pFunc)
	{
		if (pCmd == NULL)
		{
			return FALSE;
		}
		if (pFunc == NULL)
		{
			if (pCmd)
			{
				ReclaimCmd(pCmd);
			}
			return FALSE;
		}

		CMYSQLClient* pMYSQLClient = GetClientHash(nHashParm);

		if (pMYSQLClient != NULL)
		{
			CMYSQLClient::MYSQLSTRING_REQUEST* pSqlReq = pMYSQLClient->ApplySQLReq();
			pSqlReq->m_nKey = dwKey;
			pSqlReq->m_nHash = nHashParm;
			pSqlReq->m_pProcFun = pFunc;
			pSqlReq->m_pCmd = pCmd;
			pSqlReq->m_bIsAsync = TRUE;

			pMYSQLClient->RecvSQLRequest(pSqlReq);

			return  TRUE;
		}
		ReclaimCmd(pCmd);
		return  FALSE;
	}

	BOOL CMYSQLDBSystem::AddSyncSQLRequest(void* dwKey, long nHashParm, CMYSQLCommand* pCmd, MYSQLProcFunc pFunc)
	{
		if (pCmd == NULL)
		{
			return FALSE;
		}
		if (pFunc == NULL)
		{
			if (pCmd)
			{
				ReclaimCmd(pCmd);
			}
			return FALSE;
		}

		CMYSQLClient* pMYSQLClient = GetClientHash(nHashParm);

		if (pMYSQLClient != NULL)
		{
			CMYSQLClient::MYSQLSTRING_REQUEST* pSqlReq = pMYSQLClient->ApplySQLReq();
			pSqlReq->m_nKey = dwKey;
			pSqlReq->m_nHash = nHashParm;
			pSqlReq->m_pProcFun = pFunc;
			pSqlReq->m_pCmd = pCmd;
			pSqlReq->m_bIsAsync = FALSE;

			pMYSQLClient->RecvSyncSQLRequest(pSqlReq);

			return  TRUE;
		}
		ReclaimCmd(pCmd);
		return  FALSE;
	}
}