#ifndef _GLOBAL_DEFINE_H_
#define _GLOBAL_DEFINE_H_

#define CLIENT_LISTEN_PORT 0
#define USER_LISTEN_PORT 10

#ifdef LIB_WINDOWS
#define LOCAL_CONFIG_FILE	"\\YTSvr_config.ini"
#else
#define LOCAL_CONFIG_FILE	"/YTSvr_config.ini"
#endif // LIB_WINDOWS

struct sDBConnectInfo
{
	std::string m_strMySQLHost;
	std::string m_strMySQLDB;
	std::string m_strMySQLUser;
	std::string m_strMySQLPass;
	UINT m_nMySQLPort;
	UINT m_nDBThreads;
};

struct  sCacheConnectInfo
{
	std::string m_strHostname;
	std::string m_strAuthKey;
	UINT m_nPort;
};

#endif