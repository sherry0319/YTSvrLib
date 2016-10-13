// Sample.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Sample.h"
#include "global/ServerDefine.h"
#include "Config/Config.h"
#include "db/DBManager.h"
#include "GWServerParser/GWServerParser.h"
#include "db/DBManager.h"
#include "timer/TimerMgr.h"
#include "db/DBLogMgr.h"

#ifndef LIB_WINDOWS
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#endif // LIB_WINDOWS

extern YTSvrLib::CServerApplication gApp;

void DestroyAllObj()
{
	// 服务器进程销毁时处理
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
	fprintf(stderr, "signal_handle_function : %d", nSignal);
	if (nSignal == SIGSEGV || nSignal == SIGFPE || nSignal == SIGABRT)
	{
		PrintBackTrace();
		signal(nSignal,SIG_DFL);
		return;
	}
	exit(0);
}
#endif // LIB_WINDOWS

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

	gApp.GlobalInit(EAppEvent::eAppEventCount);
	//注册事件
	gApp.RegisterEvent( EAppEvent::eAppGWSvrSocketEvent, CGWSvrParser::OnMsgRecv );
	gApp.RegisterEvent( EAppEvent::eAppGWSvrSocketDisconnectEvent, CGWSvrParser::OnDisconnectMsgRecv );
	gApp.RegisterEvent( EAppEvent::eAppGameDB, CDBManager::OnDataRecv );
	gApp.RegisterEvent( EAppEvent::eAppTimerMgrOnTimer, CTimerMgr::OnTimer );

	CConfig::GetInstance();
	CDBManager::GetInstance()->SetConnection(	CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLDB.c_str(),
												CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLHost.c_str(),
												CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLUser.c_str(),
												CConfig::GetInstance()->m_sDBConnectInfo.m_strMySQLPass.c_str(),
												CConfig::GetInstance()->m_sDBConnectInfo.m_nMySQLPort,
												CConfig::GetInstance()->m_sDBConnectInfo.m_nDBThreads );


	//初始化事件
	CDBLogMgr::GetInstance();
	CTimerMgr::GetInstance()->CreateTimer(100);

	CGWSvrParser::GetInstance()->StartListen(CConfig::GetInstance()->m_nGWListenPort,
											 CConfig::GetInstance()->m_strGWListenIPAddr.c_str());

	CDBManager::GetInstance()->OnServerStart();

	gApp.Run();

	return 0;
}
