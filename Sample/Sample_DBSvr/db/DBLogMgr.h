#pragma once

#define DBLOG_FILE_ASYN				"DBLog"	//日志文件夹名称

#ifdef UTF8SQL
class CDBLogMgr : public YTSvrLib::CLogManager, public YTSvrLib::CSingle < CDBLogMgr >
#else
class CDBLogMgr : public YTSvrLib::CLogManagerW, public YTSvrLib::CSingle < CDBLogMgr >
#endif // UTF8SQL
{
public:
	CDBLogMgr();

	virtual ~CDBLogMgr()
	{

	}
};


//void DBLogASync( LPCWSTR fmt, ...);
#ifdef UTF8SQL
void DBLogASync(LPCSTR fmt);
#else
void DBLogASync(LPCWSTR fmt);
#endif // UTF8SQL
//#define DBLOG( s, ...) DBLogASync( s, __VA_ARGS__)
#define DBLOG( s ) DBLogASync( s )

void	ReOpenDBLogFile();