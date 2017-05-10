#include "stdafx.h"
#include <time.h>
#ifdef LIB_WINDOWS
#include <atlcomtime.h>
#endif // LIB_WINDOWS

#define DBCMD_AND_QUERY_APPLY(pc, pq) { pc = m_DBSys.ApplyCmd(); \
										if( pc == NULL ) {	\
										LOG("DB_ApplyCmd Error=%d!", GetLastError() );	\
										return;	\
										}	\
										pq = ApplyQueryInfo();	\
										if( pq == NULL ) {	\
										LOG("DB_ApplyQueryInfo Error=%d!", GetLastError() );	\
										m_DBSys.ReclaimCmd( pCmd );	\
										return;	\
										}	\
										}

DWORD MYSQLQueryCallBack(UINT nErrorCode, ULONG nEffectRows, void* nKey, YTSvrLib::MYSQLLIB::CMYSQLRecordSet * pResSet,BOOL bAsync)
{
#ifdef COREDEBUG
	LOG("DWORD MYSQLQueryCallBack");
#endif
	CDBManager::GetInstance()->ParseResponse(nErrorCode, nEffectRows, nKey, pResSet, bAsync);
    return 0;
}
extern YTSvrLib::CServerApplication gApp;
void CDBManager::SetEvent()
{
	gApp.SetEvent( EAppEvent::eAppGameDB );
}

BOOL CDBManager::Init()
{
	m_nMaxSaving = CConfig::GetInstance()->m_sDBConnectInfo.m_nDBThreads * 4;
	m_nCurSaving = 0;
	LOG("DB_Init MaxSaving=%d", m_nMaxSaving);
	return TRUE;
}
void CDBManager::WaitForAllRequestDone()
{
	LOG("DB_WaitForAllRequestDone...");
	BOOL bHasReq = TRUE;
	while( bHasReq || m_DBSys.IsAllDBClientIdle() == FALSE )
	{
		WaitForAllDBReqIdle();
		bHasReq = FALSE;
		if( m_lsSQLCache.empty() == false )
			bHasReq = TRUE;
		WaitForAllDBRespIdle();
	}
	LOG("DB_WaitForAllRequestDone OK.");
}

#ifdef UTF8SQL
void CDBManager::OnReqExcuteSQL(LPCSTR pwzSQL, LPCSTR pszProc, DWORD dwKey)
#else
void CDBManager::OnReqExcuteSQL(LPCWSTR pwzSQL, LPCSTR pszProc, DWORD dwKey)
#endif // UTF8SQL
{
	DBLOG(pwzSQL);
	LPSSQLInfo pSQLInfo = AllocateSQLInfo();
	if (pSQLInfo == NULL)
	{
#ifdef UTF8SQL
		LOG("DB_OnReqExcuteSQL=%s AllocateSQLInfo Error=%d!", pwzSQL, GetLastError());
#else
		char szSQL[DB_SQL_MAXLEN] = { 0 };
		WChar2Ansi(pwzSQL, szSQL, DB_SQL_MAXLEN - 1);
		szSQL[DB_SQL_MAXLEN - 1] = '\0';
		LOG("DB_OnReqExcuteSQL=%s AllocateSQLInfo Error=%d!", szSQL, GetLastError());
#endif
		return;
	}
	pSQLInfo->m_wMsgType = DBMSG_EXEC_SQL;
#ifdef UTF8SQL
	size_t nSQLLen = strlen(pwzSQL)+2;
	pSQLInfo->m_szSQL = new char[nSQLLen];
	ZeroMemory(pSQLInfo->m_szSQL,nSQLLen);
#ifdef LIB_WINDOWS
	strncpy_s(pSQLInfo->m_szSQL, nSQLLen-1, pwzSQL, nSQLLen-1);
#else
	strncpy_s(pSQLInfo->m_szSQL,pwzSQL, nSQLLen-1);
#endif // LIB_WINDOWS
#else
	size_t nSQLLen = wcslen(pwzSQL)+2;
	pSQLInfo->m_wzSQL = new wchar_t[nSQLLen];
	ZeroMemory(pSQLInfo->m_wzSQL,sizeof(wchar_t)*nSQLLen);
#ifdef LIB_WINDOWS
	wcsncpy_s(pSQLInfo->m_wzSQL,nSQLLen-1, pwzSQL, nSQLLen-1);
#else
	wcsncpy_s(pSQLInfo->m_wzSQL,pwzSQL, nSQLLen - 1);
#endif
#endif // UTF8SQL
	strncpy_s(pSQLInfo->m_szProcName, pszProc, DB_PROCNAME_MAXLEN - 1);
	pSQLInfo->m_szProcName[DB_PROCNAME_MAXLEN - 1] = '\0';
	pSQLInfo->m_dwKey = dwKey;
	
	if (m_nCurSaving >= m_nMaxSaving)
	{
		m_lsSQLCache.push_back(pSQLInfo);
		return;
	}
	ExcuteSQL(pSQLInfo);
}

void CDBManager::ExcuteSQL( LPSSQLInfo pSQLInfo )
{
	YTSvrLib::CMYSQLCommand* pCmd = NULL;
	YTSvrLib::CMYSQLQueryInfo*  pQueryInfo = NULL;
	BOOL bRes = ApplyCmdAndQueryInfo(&pCmd,&pQueryInfo);
	if (bRes == FALSE)
	{
		LOG("Allocate Cmd And QueryInfo Failed");
		return;
	}

#ifdef UTF8SQL
	pCmd->AddQueryText_NoFormat(pSQLInfo->m_szSQL);
#else
	pCmd->AddQueryText_NoFormat(pSQLInfo->m_wzSQL);
#endif // UTF8SQL
	pCmd->SetExcute();

	strncpy_s( pQueryInfo->m_strSPName, pSQLInfo->m_szProcName, 63  );
	pQueryInfo->m_nType = eExecSQL;
	pQueryInfo->m_ayParam[0] = (UINT_PTR)pSQLInfo;
	m_DBSys.AddSQLRequest( pQueryInfo,pSQLInfo->m_dwKey,pCmd,MYSQLQueryCallBack );
	m_nCurSaving++;
}

void CDBManager::OnExcuteSQLEnd( YTSvrLib::CMYSQLQueryInfo* pQueryInfo, YTSvrLib::MYSQLRESPONSEINFO* pRespInfo )
{
	if( GetInstance()->m_nCurSaving > 0 )
		GetInstance()->m_nCurSaving--;
	LPSSQLInfo pSQLInfo = (LPSSQLInfo)pQueryInfo->m_ayParam[0];
	if( pRespInfo->nErrorCode != 0 )
	{
		if( pSQLInfo )
		{
			LOG("DB_OnExcuteSQLEnd Proc=%s Msg=%d Error=%d!", pQueryInfo->m_strSPName, pSQLInfo->m_wMsgType, pRespInfo->nErrorCode );
			
			GetInstance()->ReleaseSQLInfo( pSQLInfo );

			pSQLInfo = NULL;
		}
		else
		{
			LOG( "DB_OnExcuteSQLEnd fail, err code: %d, %s", pRespInfo->nErrorCode, pQueryInfo->m_strSPName );
		}
	}
	else
	{
		if (pSQLInfo)
		{
			GetInstance()->ReleaseSQLInfo( pSQLInfo );	
			pSQLInfo = NULL;
		}
	}

	if( pSQLInfo == NULL )
	{
		if (GetInstance()->m_lsSQLCache.empty())
		{
			return;
		}
		pSQLInfo = GetInstance()->m_lsSQLCache.pop_front();
	}
	if( pSQLInfo )
	{
		switch( pSQLInfo->m_wMsgType )
		{
		case DBMSG_EXEC_SQL:
			{
				GetInstance()->ExcuteSQL(pSQLInfo);
			}
			break;
		default:
			{
				LOG("DB_OnExcuteSQLEnd Invalid MsgType=%d Error!", pSQLInfo->m_wMsgType );
				GetInstance()->ReleaseSQLInfo( pSQLInfo );
				break;
			}
		}
	}
	else
	{
		LOG("m_lsSQLCache.pop_front() pSQLInfo == NULL");
	}
}

void CDBManager::CheckExcuteSQLQueue()
{
	if (m_nLastCheckCount != 0 
		&& m_lsSQLCache.size() != 0 
		&& (int)m_lsSQLCache.size() >= m_nLastCheckCount
		&& m_nLastCheckCount >= 100000)
	{
		SetEvent();
	}
	m_nLastCheckCount = (int) m_lsSQLCache.size();
}