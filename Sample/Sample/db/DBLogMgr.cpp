#include "stdafx.h"
#include "DBLogMgr.h"

CDBLogMgr::CDBLogMgr()
{
	strncpy_s(m_wzFilePath, DBLOG_FILE_ASYN, 127);
	strncpy_s(m_szFileExt, "sql", 15);

	m_hAsynFileHandle = OpenAsynLogFile(DBLOG_FILE_ASYN);
}
//void DBLogASync( LPCWSTR fmt, ...)
#ifdef UTF8SQL
void DBLogASync(LPCSTR fmt)
{
	char cTime[128] = {0};
	GetDateTime( cTime, 127, 'A');
	cTime[127] = '\0';

	LPCSTR lpwzComment = "#";

	YTSvrLib::CLogBufferA* pLogBuffer = CDBLogMgr::GetInstance()->ApplyObj();
	char* lpwzWriteBuf = pLogBuffer->GetBuffer();
#ifdef LIB_WINDOWS
	pLogBuffer->ReSize(
		_snprintf_s(lpwzWriteBuf, pLogBuffer->GetCapcity(), pLogBuffer->GetCapcity(), "%s[%s]\r\n%s\r\n",
		lpwzComment, cTime, fmt)
		);
#else
	pLogBuffer->ReSize(
		_snprintf_s(lpwzWriteBuf, pLogBuffer->GetCapcity(), "%s[%s]\n%s\n",lpwzComment, cTime, fmt));
#endif // LIB_WINDOWS
	_ASSERT( strlen(lpwzWriteBuf) );
	CDBLogMgr::PostBufferReady(CDBLogMgr::GetInstance(), pLogBuffer, YTSvrLib::IOErrorData);
}
#else
void DBLogASync( LPCWSTR fmt )
{
	wchar_t cTime[128] = {0};
	GetDateTimeW( cTime, 127, 'A');
	cTime[127] = L'\0';

	LPCWSTR lpwzComment = L"#";
	
	YTSvrLib::CLogBufferW* pLogBuffer = CDBLogMgr::GetInstance()->ApplyObj();
	wchar_t* lpwzWriteBuf = pLogBuffer->GetBuffer();
#ifdef LIB_WINDOWS
	pLogBuffer->ReSize(
		_snwprintf_s(lpwzWriteBuf, pLogBuffer->GetCapcity(), pLogBuffer->GetCapcity(), L"%s[%s]\r\n%s\r\n",
		lpwzComment, cTime, fmt)
		);
#else
	pLogBuffer->ReSize(
		_snwprintf_s(lpwzWriteBuf, pLogBuffer->GetCapcity(), L"%ls[%ls]\r\n%ls\r\n",lpwzComment, cTime, fmt));
#endif // LIB_WINDOWS
	_ASSERT( wcslen(lpwzWriteBuf) );
	CDBLogMgr::PostBufferReady(CDBLogMgr::GetInstance(), pLogBuffer, YTSvrLib::IOErrorData);
}
#endif // UTF8SQL

void ReOpenDBLogFile()
{
	CDBLogMgr::GetInstance()->ReOpenAsynLogFile();
}