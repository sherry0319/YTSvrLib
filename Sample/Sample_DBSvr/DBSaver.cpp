// DBSaver.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <assert.h>
#include "DBSaver.h"

#ifndef LIB_WINDOWS
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#endif // LIB_WINDOWS

// 唯一的应用程序对象
void DestroyAllObj()
{
	CGMSvrParser::GetInstance()->StopListen();
	CDBCache::GetInstance()->RefreshSQLCache();
	CDBManager::GetInstance()->WaitForAllRequestDone();
}
#ifdef LIB_WINDOWS
BOOL WINAPI ConsoleHandler(DWORD dwCtrlType)
{
	if (dwCtrlType == CTRL_CLOSE_EVENT || dwCtrlType == CTRL_C_EVENT
		|| dwCtrlType == CTRL_BREAK_EVENT)
	{
		LOG("System is Closing...");
		DestroyAllObj();
		LOG("System is Closed...");
		ExitProcess(0);
		//  return FALSE;
	}
	return TRUE;
}
#else
void signal_handle_function(int nSignal)
{
	LOG("signal_handle_function : %d", nSignal);
	DestroyAllObj();
	if (nSignal == SIGSEGV || nSignal == SIGFPE || nSignal == SIGABRT)
	{
		PrintBackTrace();
		signal(nSignal,SIG_DFL);
		return;
	}
	exit(0);
}
#endif // LIB_WINDOWS

extern YTSvrLib::CServerApplication gApp;

#ifdef LIB_WINDOWS
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
#else
int main(int argc, char* argv[])
#endif //
{
#ifdef LIB_WINDOWS
	SetConsoleCtrlHandler(ConsoleHandler, true);
#else
	SetConsoleCtrlHandler(signal_handle_function);
#endif // LIB_WINDOWS

	//注册事件
	gApp.RegisterEvent( EAppEvent::eAppGMSvrSocketEvent, CGMSvrParser::OnMsgRecv );
	gApp.RegisterEvent( EAppEvent::eAppGMSvrSocketDisconnectEvent, CGMSvrParser::OnDisconnectMsgRecv );
	gApp.RegisterEvent( EAppEvent::eAppGameDB, CDBManager::OnDataRecv );
	gApp.RegisterEvent(EAppEvent::eAppTimerMgrOnTimer, CTimerMgr::OnTimer );

	//读取配置
	CConfig::GetInstance();
	CTimerMgr::GetInstance()->CreateTimer(500);

	//初始化事件
	if (CConfig::GetInstance()->IsInitSQLExist())
	{// 如果需要初始化执行存储.则需要先准备好数据库引擎.加载完初始化执行.再开启监听
		CDBManager::GetInstance()->SetConnection(
			CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLDB.c_str(),
			CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLHost.c_str(),
			CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLUser.c_str(),
			CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLPass.c_str(),
			CConfig::GetInstance()->m_sDBConnectInfo.m_nMySQLPort,
			CConfig::GetInstance()->m_sDBConnectInfo.m_nDBThreads,
			CConfig::GetInstance()->m_nDBRetryCount);
		CDBManager::GetInstance()->Init();
		CConfig::GetInstance()->LoadIniSQL();
		CGMSvrParser::GetInstance()->StartListen(CConfig::GetInstance()->m_nGMSvrListenPort, CConfig::GetInstance()->m_strGMSvrListenIPAddr.c_str());
	}
	else
	{// 否则可以先开启监听以加快启动速度
		CGMSvrParser::GetInstance()->StartListen(CConfig::GetInstance()->m_nGMSvrListenPort, CConfig::GetInstance()->m_strGMSvrListenIPAddr.c_str());
		CDBManager::GetInstance()->SetConnection(
			CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLDB.c_str(),
			CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLHost.c_str(),
			CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLUser.c_str(),
			CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLPass.c_str(),
			CConfig::GetInstance()->m_sDBConnectInfo.m_nMySQLPort,
			CConfig::GetInstance()->m_sDBConnectInfo.m_nDBThreads,
			CConfig::GetInstance()->m_nDBRetryCount);
		CDBManager::GetInstance()->Init();
	}

	gApp.Run();
	LOG("System is closing...");
	CDBManager::GetInstance()->WaitForAllRequestDone();

	return 0;
}