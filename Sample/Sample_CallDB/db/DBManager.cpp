#include "stdafx.h"
#include <time.h>
#include "DBManager.h"
#include "../global/ServerDefine.h"
#include "../Config/Config.h"
#include "../timer/TimerMgr.h"
#include "../GWServerParser/GWServerParser.h"

DWORD MYSQLQueryCallBack(UINT nErrorCode, ULONG nEffectRows, void* nKey, YTSvrLib::MYSQLLIB::CMYSQLRecordSet * pResSet, BOOL bAsync)
{
	CDBManager::GetInstance()->ParseResponse(nErrorCode, nEffectRows, nKey, pResSet, bAsync);

	return 0;
}

extern YTSvrLib::CServerApplication gApp;

CDBManager::CDBManager():CMYSQLManagerBase()
{
#undef DB_PARSE
#define DB_PARSE(emID,func) Register( emID, func, "" );
#include "DBParseTable.h"
}

void CDBManager::SetEvent()
{
	gApp.SetEvent(EAppEvent::eAppGameDB);
}

BOOL CDBManager::ApplyCmdAndQueryInfo(YTSvrLib::CMYSQLCommand** ppCmd, YTSvrLib::CMYSQLQueryInfo** ppQueryInfo)
{
	*ppCmd = m_DBSys.ApplyCmd();
	if (*ppCmd == NULL)
	{
		LOG("DB_ApplyCmd Error=%d!", GetLastError());
		return FALSE;
	}
	*ppQueryInfo = ApplyQueryInfo();
	if (*ppQueryInfo == NULL)
	{
		LOG("DB_ApplyQueryInfo Error=%d!", GetLastError());
		m_DBSys.ReclaimCmd(*ppCmd);
		return FALSE;
	}
	return TRUE;
}

void CDBManager::SetConnection(LPCSTR lpszDB, LPCSTR lpszSvr, LPCSTR lpszUser, LPCSTR lpszPass, UINT nPort, UINT nCount, UINT nRetryCount /*= 0*/)
{
	if (nCount <= 0)
		nCount = 4;
	for (UINT i = 0; i < nCount; i++)
	{
		m_DBSys.AddServerConnectInfo(lpszSvr, lpszUser, lpszPass, lpszDB, nPort,60);
	}

	m_DBSys.Startup(nRetryCount, TRUE);
}

void CDBManager::OnServerStart()
{
	DoQuery("call p_test();","p_test", eQueryTest);
}

void CDBManager::DoQuery(const char* pSQL, const char* pSPName, ECallBackType eCallBackType,
							  LONGLONG wParam /*= 0*/, LONGLONG lParam /*= 0*/, LONGLONG kParam /*= 0*/, 
							  LONGLONG xParam /*= 0*/, LONGLONG yParam /*= 0*/, LONGLONG zParam /*= 0*/, BOOL bASync /*= TRUE*/)
{
	DBLOG(pSQL);// 记录执行的SQL

	YTSvrLib::CMYSQLCommand* pCmd = NULL;
	YTSvrLib::CMYSQLQueryInfo*  pQueryInfo = NULL;
	BOOL bRes = ApplyCmdAndQueryInfo(&pCmd, &pQueryInfo);
	if (bRes == FALSE)
	{
		LOGWARN("Allocate Cmd And QueryInfo Failed");
		return;
	}

	pCmd->AddQueryText_NoFormat(pSQL);

	strncpy_s(pQueryInfo->m_strSPName, pSPName, 63);
	pQueryInfo->m_nType = eCallBackType;
	pQueryInfo->m_ayParam[0] = wParam;
	pQueryInfo->m_ayParam[1] = lParam;
	pQueryInfo->m_ayParam[2] = kParam;
	pQueryInfo->m_ayParam[3] = xParam;
	pQueryInfo->m_ayParam[4] = yParam;
	pQueryInfo->m_ayParam[5] = zParam;

	if (bASync)
	{// 异步执行
		m_DBSys.AddSQLRequest(pQueryInfo, eCallBackType, pCmd, MYSQLQueryCallBack );
	}
	else
	{// 同步执行
		m_DBSys.AddSyncSQLRequest(pQueryInfo, eCallBackType, pCmd, MYSQLQueryCallBack);
	}
}

void CDBManager::OnQueryTest(YTSvrLib::CMYSQLQueryInfo* pQueryInfo, YTSvrLib::MYSQLRESPONSEINFO* pRespInfo)
{
	LONGLONG wParam = pQueryInfo->m_ayParam[0];
	LONGLONG lParam = pQueryInfo->m_ayParam[1];
	LONGLONG kParam = pQueryInfo->m_ayParam[2];
	LONGLONG xParam = pQueryInfo->m_ayParam[3];
	LONGLONG yParam = pQueryInfo->m_ayParam[4];
	LONGLONG zParam = pQueryInfo->m_ayParam[5];

	if (pRespInfo->nErrorCode != 0)
	{
		LOGWARN("DB OnQueryTest fail, err code: %d, %s", pRespInfo->nErrorCode, pQueryInfo->m_strSPName);
		return;
	}

	long nCount = pRespInfo->pResSet->GetFieldCount();
	if (nCount < 1)
	{
		LOGWARN("DB OnQueryTest fail Field=%d", nCount);
		return;
	}
	try
	{
		while (pRespInfo->pResSet->IsEof() == FALSE)
		{
// 				id
			int nID = pRespInfo->pResSet->GetFieldValue<int>("id");
// 				string_param
			char szStringParam[256] = { 0 };
			strncpy_s(szStringParam, pRespInfo->pResSet->GetFieldValue<LPCSTR>("string_param"), 255);
// 				double_param
			double dDoubleParam = pRespInfo->pResSet->GetFieldValue<double>("double_param");
// 				datetime_param
			__time32_t tTimeParam = (__time32_t) pRespInfo->pResSet->GetFieldValue<CSQLDateTime>("datetime_param");
// 				bigint_param
			LONGLONG llBigintParam = pRespInfo->pResSet->GetFieldValue<LONGLONG>("bigint_param");
// 				blob_param
			char szBlobParam[1024] = { 0 };
			int nBloblen = pRespInfo->pResSet->GetFieldValueBinary("blob_param", szBlobParam, 1023);

			pRespInfo->pResSet->MoveNext();
		}
	}
	catch (YTSvrLib::CMYSQLException& e)
	{
		LOGWARN("OnQueryTest Exception %s(%d)", e.GetError(), e.GetErrorCode());
		return;
	}
}

void CDBManager::SendSomeQuery(CGWSvrSocket* pGWSocket)
{
	YTSvrLib::CMYSQLCommand* pCmd = NULL;
	YTSvrLib::CMYSQLQueryInfo*  pQueryInfo = NULL;
	BOOL bRes = ApplyCmdAndQueryInfo(&pCmd, &pQueryInfo);
	if (bRes == FALSE)
	{
		LOGWARN("Allocate Cmd And QueryInfo Failed");
		return;
	}

	pCmd->AddQueryText_NoFormat("call p_somecall();");

	strncpy_s(pQueryInfo->m_strSPName, "p_somecall", 63);
	pQueryInfo->m_nType = eQuerySome;
	pQueryInfo->m_ayParam[0] = (LONGLONG)pGWSocket;

	m_DBSys.AddSQLRequest(pQueryInfo, eQuerySome, pCmd, MYSQLQueryCallBack);
}

void CDBManager::OnQuerySome(YTSvrLib::CMYSQLQueryInfo* pQueryInfo, YTSvrLib::MYSQLRESPONSEINFO* pRespInfo)
{
	if (pRespInfo->nErrorCode == 0)
	{
		CGWSvrSocket* pSocket = (CGWSvrSocket*)pQueryInfo->m_ayParam[0];
		if (pSocket)
		{
			// send back result to client
			const char* message = "database query succ";

			CGWSvrParser::GetInstance()->SendSvrMsg(pSocket, message, (int)strlen(message));
		}
	}
}