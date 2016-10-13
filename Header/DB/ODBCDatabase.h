#pragma once
#ifndef _ITOUCH_ODBC_DATABASE_H__
#define _ITOUCH_ODBC_DATABASE_H__

#define ODBC_MAXLEN		63
//#import "c:\program files\common files\system\ado\msado25.tlb" no_namespace rename ("EOF", "adoEOF")
#include <Windows.h>
#include <Sqltypes.h>
#include <Odbcinst.h>
#include <Sql.h>
#include <Sqlext.h>
#include <Sqlucode.h>
#include <Msdasql.h>
#include <Msdadc.h>
#include <odbcss.h>

class CODBCDatabase
{
public:
	CODBCDatabase();
	~CODBCDatabase();

	BOOL Close();
	BOOL SetConnection( LPSTR pszDSN, LPSTR pszUser, LPSTR pszPwd );
	BOOL Connect();
	BOOL ReConnect();
//	BOOL DriverConnect(LPCSTR pszConnect = NULL);
	BOOL IsConnected();

	BOOL ExecSQL( LPSTR pszSql );

	
//////////////////////////////////////////////////////////////////////////
	
protected:
	char	m_szODBCDSN[ODBC_MAXLEN+1];
	char	m_szODBCUser[ODBC_MAXLEN+1];
	char	m_szODBCPwd[ODBC_MAXLEN+1];

	SQLHDBC			m_hDbc;
	SQLHENV			m_hEnv;
	BOOL			m_bIsConnected;

};


time_t WINAPI SQLTimeStamp2Time_t( TIMESTAMP_STRUCT * ptsSQL );

#endif