#include "stdafx.h"
#include "Config.h"

CConfig::CConfig(void)
{
	BOOL bRes = ReadConfig();
	if (FALSE == bRes)
	{
		getchar();
		ExitProcess(0);
	}
}

CConfig::~CConfig(void)
{
}

BOOL CConfig::ReadConfig()
{
	memset(m_szReadConfigFile, 0, sizeof(m_szReadConfigFile));
	GetModuleFilePath(m_szReadConfigFile, MAX_PATH);
	strcat_s(m_szReadConfigFile, MAX_PATH, LOCAL_CONFIG_FILE);

	YTSvrLib::CConfigLoader cfgLocal(m_szReadConfigFile);
	LOG("Initializing basic config......");
	cfgLocal.SetSection("Common");

	m_nPublicSvrID = cfgLocal.ParseInt("Public ServerID", 0);
	m_nLocalSvrID = cfgLocal.ParseInt("Local ServerID", 0);
	m_nGameID = cfgLocal.ParseInt("Game ID");

	LOG("Initializing game mysql config......");
	cfgLocal.SetSection("MYSQLDB");
	m_sDBConnectInfo.m_strMySQLHost = cfgLocal.ParseString("DBHostname", "127.0.0.1");
	m_sDBConnectInfo.m_strMySQLDB = cfgLocal.ParseString("DefaultDB", "");
	m_sDBConnectInfo.m_strMySQLUser = cfgLocal.ParseString("DBUser", "root");
	m_sDBConnectInfo.m_strMySQLPass = cfgLocal.ParseString("DBPass", "");
	m_sDBConnectInfo.m_nMySQLPort = cfgLocal.ParseInt("DBPort", 3306);
	m_sDBConnectInfo.m_nDBThreads = cfgLocal.ParseInt("DBThread", 4);

	cfgLocal.SetSection("Sample Network");
	m_strGWListenIPAddr = cfgLocal.ParseString("Sample Listen IP", "");
	if (m_strGWListenIPAddr.empty())
		m_strGWListenIPAddr = "0.0.0.0";//¼àÌýµÄÄ¬ÈÏÊÇINADDR_ANY
	m_nGWListenPort = cfgLocal.ParseInt("Sample Listen Port", m_nLocalSvrID + USER_LISTEN_PORT);

	return TRUE;
}