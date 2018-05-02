#include "stdafx.h"
#include "Config.h"

CConfig::CConfig(void)
{
	m_bCfgInited = FALSE;
	ReadConfig();
}

CConfig::~CConfig(void)
{
}

BOOL CConfig::ReadConfig()
{
	char szDir[MAX_PATH] = {0};
	GetModuleFilePath(szDir, 255);
	strncat( szDir, LOCAL_CONFIG_FILE ,255);

	YTSvrLib::CConfigLoader cfgLocal(szDir);
	cfgLocal.SetSection( "Common" );
	m_nLocalSvrID = cfgLocal.ParseInt( "Local ServerID", 0 );
	m_nPublicSvrID = cfgLocal.ParseInt( "Public ServerID", 0 );
	m_nGameID = cfgLocal.ParseInt( "Game ID" );
	m_nMinClientMsgToZip = cfgLocal.ParseInt( "Min Data To Zip", 0 );
	m_strWarningPage = cfgLocal.ParseString("WarningPage");

	m_strWarningDest = cfgLocal.ParseString("WarningDest");

	cfgLocal.SetSection( "Gateway Network" );
	m_strClientListenIPAddr = cfgLocal.ParseString( "Client Listen IP", "" );
	if( m_strClientListenIPAddr.empty() )
		m_strClientListenIPAddr = "0.0.0.0";
	m_nClientListenPort = cfgLocal.ParseInt( "Client Listen Port", m_nLocalSvrID );

	std::vector<std::string> vctStr;
	string strClientWhiteList = cfgLocal.ParseString("Client White List");
	StrDelimiter(strClientWhiteList,"|",vctStr);
	m_vctClientIPWhiteList.clear();
	for( UINT i=0; i<vctStr.size(); i++ )
		m_vctClientIPWhiteList.push_back( inet_addr( vctStr[i].c_str()) );

	cfgLocal.SetSection("UserServer Network");
	m_strUserSvrConnectIP = cfgLocal.ParseString( "UserServer Connect IP", "" );
	if (m_strUserSvrConnectIP.empty())
	{
		m_strUserSvrConnectIP = "127.0.0.1";
	}
	m_nUserSvrListenPort = cfgLocal.ParseInt( "UserServer Listen Port", m_nLocalSvrID+USER_LISTEN_PORT );

	cfgLocal.SetSection("UserServer Network 2");
	m_strUserSvrConnectIP2 = cfgLocal.ParseString("UserServer Connect IP", "");
	if (m_strUserSvrConnectIP2.empty())
	{
		m_strUserSvrConnectIP2 = "127.0.0.1";
	}
	m_nUserSvrListenPort2 = cfgLocal.ParseInt("UserServer Listen Port", m_nLocalSvrID + USER_LISTEN_PORT + USER_LISTEN_PORT);

	if (m_nClientListenPort != m_nLocalSvrID)
	{
		m_nLocalSvrID = m_nClientListenPort;
	}

	m_bCfgInited = TRUE;

	return TRUE;
}

