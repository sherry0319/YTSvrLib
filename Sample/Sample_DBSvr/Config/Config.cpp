#include "stdafx.h"
#include <vector>
#include "Config.h"
//////////////////////////////////////////////////////////////////////////
CConfig::CConfig(void)
{
	ReadConfig();
}

CConfig::~CConfig(void)
{

}

BOOL CConfig::ReadConfig()
{
	char szDir[MAX_PATH] = {0};
	GetModuleFilePath(szDir, MAX_PATH);
	strncat( szDir, LOCAL_CONFIG_FILE,MAX_PATH );
	
	YTSvrLib::CConfigLoader cfgLocal(szDir);
	cfgLocal.SetSection("Common");
	m_nLocalSvrID = cfgLocal.ParseInt("Local ServerID");
	m_nPublicSvrID = cfgLocal.ParseInt( "Public ServerID", 0 );
	m_strWarningDest = cfgLocal.ParseString("WarningDest");
	m_strWarningPage = cfgLocal.ParseString("WarningPage");


	cfgLocal.SetSection( "MYSQLDB" );
	m_sDBConnectInfo.m_strMySQLHost = cfgLocal.ParseString( "DBHostname", "127.0.0.1" );
	m_sDBConnectInfo.m_strMySQLDB = cfgLocal.ParseString( "DefaultDB", "" );
	m_sDBConnectInfo.m_strMySQLUser = cfgLocal.ParseString( "DBUser", "root" );
	m_sDBConnectInfo.m_strMySQLPass = cfgLocal.ParseString( "DBPass", "" );
	m_sDBConnectInfo.m_nMySQLPort = cfgLocal.ParseInt( "DBPort" ,3306);
	m_sDBConnectInfo.m_nDBThreads = cfgLocal.ParseInt( "DBThread" ,4);
	
	cfgLocal.SetSection( "DBServer Network" );
	m_strGMSvrListenIPAddr = cfgLocal.ParseString( "DBServer Listen IP", "" );
	if( m_strGMSvrListenIPAddr.empty() )
		m_strGMSvrListenIPAddr = "0.0.0.0";
	m_nGMSvrListenPort = cfgLocal.ParseInt( "DBServer Listen Port", m_nLocalSvrID+DB_LISTEN_PORT );
	m_nDBRetryCount = cfgLocal.ParseInt( "DBServer Retry Count", 0 );

	cfgLocal.SetSection("Game Config");
	m_bIsSQLCache = cfgLocal.ParseInt("SQLCache", 1);
	m_nSQLCacheCleanLine = cfgLocal.ParseInt("SQLCacheLine", 1000);

	
	m_vctGMSvrIPWhiteList.clear();
	std::vector<std::string> vctStr;
	string strSvrWhiteList = cfgLocal.ParseString("DBServer White List");
	StrDelimiter(strSvrWhiteList,"|",vctStr);
	for( UINT i=0; i<vctStr.size(); i++ )
		m_vctGMSvrIPWhiteList.push_back( inet_addr( vctStr[i].c_str()) );

	int nUserDBThreads = cfgLocal.ParseInt("DB Threads");
	if (nUserDBThreads > 0)
	{
		m_sDBConnectInfo.m_nDBThreads = nUserDBThreads;
	}
	else
	{
		m_sDBConnectInfo.m_nDBThreads = GetCPUCoreCount();
	}

	ZeroMemory(m_szModuleName,sizeof(m_szModuleName));
	GetModuleFileName(m_szModuleName, 127);

	return TRUE;
}

BOOL CConfig::IsInitSQLExist()
{
	char szDir[MAX_PATH] = { 0 };
	GetModuleFilePath(szDir, MAX_PATH);
	strncat(szDir, DIRECTORY_SEPARATOR "init.sql", MAX_PATH);

	FILE* pFile = fopen(szDir, "r");
	if (pFile == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CConfig::LoadIniSQL()
{
	char szDir[MAX_PATH] = { 0 };
	GetModuleFilePath(szDir, MAX_PATH);
	strncat(szDir, DIRECTORY_SEPARATOR "init.sql", MAX_PATH);

	FILE* pFile = fopen(szDir,"r");
	if (pFile == NULL)
	{
		return FALSE;
	}

	fseek(pFile, 0, SEEK_END);
	size_t nSize = ftell(pFile);
	LOG("LoadIniSQL : Size=[%d]",nSize);
	fseek(pFile, 0, SEEK_SET);
	char* file = new char[nSize + 1];
	ZeroMemory(file, (size_t) (nSize + 1));
	fread(file, 1, nSize, pFile);
	fclose(pFile);

	file[nSize] = '\0';

	int nIndex = 0;

	char szSQL[4096] = { 0 };
	int nPos = 0;
	char* pos = file;
	while (*pos != '\0')
	{
		szSQL[nPos] = *pos;
		if (*pos == '\n')
		{
			if (szSQL[0] != '#' && szSQL[0] != '\0')
			{
				if (*(pos - 1) == ';' || (*(pos - 1) == '\r' && *(pos - 2) == ';'))
				{
					if (szSQL[nPos] == '\n')
					{
						szSQL[nPos] = 0;
					}
					CDBManager::GetInstance()->OnReqExcuteSQL(szSQL, "", nIndex++);
					
					ZeroMemory(szSQL, sizeof(szSQL));
					nPos = 0;
					pos++;
				}
				else
				{
					nPos++;
					pos++;
				}
			}
			else if (szSQL[0] == '#')
			{
				ZeroMemory(szSQL, sizeof(szSQL));
				nPos = 0;
				pos++;
			}
			continue;
		}
		nPos++;
		pos++;
	}

	delete[] file;

	unlink(szDir);

	return TRUE;
}