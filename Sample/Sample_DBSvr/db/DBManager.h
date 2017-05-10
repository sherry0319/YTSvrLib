#pragma once

#include "../GMSvrParser/GMSvrDef.h"
#include "DBLogMgr.h"

struct sSQLInfo : public YTSvrLib::CRecycle
{
	WORD m_wMsgType;

	char	m_szProcName[DB_PROCNAME_MAXLEN];
#ifdef UTF8SQL
	char*		m_szSQL;
#else
	wchar_t*	m_wzSQL;
#endif // UTF8SQL
	DWORD	m_dwKey;

	sSQLInfo()
	{
		m_wMsgType = 0;
		ZeroMemory(m_szProcName, sizeof(m_szProcName));
#ifdef UTF8SQL
		m_szSQL = NULL;
#else
		m_wzSQL = NULL;
#endif // UTF8SQL
		m_dwKey = 0;
	}

	virtual void Init()
	{
		m_wMsgType = 0;
		ZeroMemory(m_szProcName, sizeof(m_szProcName));
#ifdef UTF8SQL
		if (m_szSQL)
		{
			delete[] m_szSQL;
		}
		m_szSQL = NULL;
#else
		if (m_wzSQL)
		{
			delete[] m_wzSQL;
		}
		m_wzSQL = NULL;
#endif // UTF8SQL
		m_dwKey = 0;
	}
};
typedef sSQLInfo* LPSSQLInfo;

class CDBManager:public YTSvrLib::CMYSQLManagerBase,public YTSvrLib::CSingle<CDBManager>
{
public:
    CDBManager():YTSvrLib::CMYSQLManagerBase(), m_poolSQL("sSQLInfo")
	//#endif
    {
		Register( eExecSQL, OnExcuteSQLEnd, "" );
    }
    ~CDBManager(){}

public:
    enum ECallBackType
    {
        eInvalid,
		eExecSQL,
        eCount //Max 255
    };

public:
    void SetConnection( LPCSTR lpszDB,LPCSTR lpszSvr,LPCSTR lpszUser,LPCSTR lpszPass,UINT nPort,UINT nCount ,UINT nRetryCount)
    {
		if( nCount <= 0 )
			nCount = 4;
		for( UINT i=0; i<nCount; i++ )
		{
			m_DBSys.AddServerConnectInfo( lpszSvr,lpszUser,lpszPass,lpszDB,nPort,10,0);
		}

        m_DBSys.Startup( nRetryCount);
    }

	BOOL ApplyCmdAndQueryInfo( YTSvrLib::CMYSQLCommand** ppCmd, YTSvrLib::CMYSQLQueryInfo** ppQueryInfo )
	{
		*ppCmd = m_DBSys.ApplyCmd();
		if( *ppCmd == NULL )
		{
			LOG("DB_ApplyCmd Error=%d!", GetLastError() );
			return FALSE;
		}
		*ppQueryInfo = ApplyQueryInfo();
		if( *ppQueryInfo == NULL )
		{
			LOG("DB_ApplyQueryInfo Error=%d!", GetLastError() );
			m_DBSys.ReclaimCmd( *ppCmd );
			return FALSE;
		}
		return TRUE;
	}
	static void OnDataRecv()
	{
#ifdef COREDEBUG
		LOG("static libevent callback OnDataRecv");
#endif
		GetInstance()->OnDataReceive();
	}

    virtual void SetEvent();
	void	WaitForAllRequestDone();
	BOOL	Init();

#ifdef UTF8SQL
	void	OnReqExcuteSQL( LPCSTR pwzSQL, LPCSTR pszProc, DWORD dwKey );
#else
	void	OnReqExcuteSQL(LPCWSTR pwzSQL, LPCSTR pszProc, DWORD dwKey);
#endif // UTF8SQL
	void	ExcuteSQL( LPSSQLInfo pSQLInfo );
	static void OnExcuteSQLEnd( YTSvrLib::CMYSQLQueryInfo* pQueryInfo, YTSvrLib::MYSQLRESPONSEINFO* pRespInfo );
	UINT	GetExcuteSQLQueueLen() { return m_lsSQLCache.size(); }
	
	LPSSQLInfo AllocateSQLInfo() { 
		return m_poolSQL.ApplyObj();
	};
	void ReleaseSQLInfo( LPSSQLInfo pObj ) {
		m_poolSQL.ReclaimObj( pObj );
	};

	__time32_t GetLastSendGSM(){return m_tLastSendGSM;}
	void SetLastSendGSM(__time32_t tTime){m_tLastSendGSM = tTime;}

	void CheckExcuteSQLQueue();
private:
	YTSvrLib::CPool<sSQLInfo, 128> m_poolSQL;
	UINT m_nMaxSaving;
	UINT m_nCurSaving;
	YTSvrLib::CWQueue<LPSSQLInfo> m_lsSQLCache;

	__time32_t m_tLastSendGSM;
	int m_nLastCheckCount;
};