// DBThread.h: interface for the CDBThread class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ThreadMgr.h"
#include "ODBCDatabase.h"



class CDBThread : public CThreadMgr 
{

    CDBThread();
    virtual ~CDBThread();


    static CDBThread* m_pInstance;

public:

    static CDBThread* GetInstance()
    {
        if( m_pInstance )
        {
            return m_pInstance;
        }
        m_pInstance = new CDBThread;
        return m_pInstance;
    }

    BOOL Init();
    BOOL Uninit();
    BOOL OnTimer();

    void SetODBC( LPSTR pszODBCDSN, LPSTR pszODBCUser, LPSTR pszODBCPwd );
    UINT GetCmdList() { return GetCommandListCount(); };

protected:

    BOOL ProcessCommand(const SCommand* pCmd);
    void ProcessException(PEXCEPTION_RECORD pEptRec, PCONTEXT pCxtRec);

    BOOL OpenDB();
    BOOL CloseDB();

private:

    CODBCDatabase m_db;
};

//void ReportError(SQLHSTMT &hstmt, int iHandleType ,CString strAlert);

BOOL WINAPI InitDBThreads();
BOOL WINAPI UninitDBThreads();
BOOL WINAPI PostDBThreadCmd(UINT nCmd, WPARAM wParam, LPARAM lParam);
BOOL WINAPI PostDBThreadCmd( UINT nID, UINT nCmd, WPARAM wParam, LPARAM lParam);
BOOL WINAPI AllDBThreadsBeIdle();

UINT WINAPI GetDBThreadCount();
CDBThread* WINAPI GetDBThread( int nID );



//#endif // !defined(AFX_DBTHREAD_H__D3BA5434_7009_48C4_97E4_D821F12E01C8__INCLUDED_)
