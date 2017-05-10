#pragma once

class CConfig : public YTSvrLib::CSingle<CConfig>
{
public:
    CConfig(void);
    virtual ~CConfig(void);
public: 
    //////////////////////////////////////////////////////////////////////////
	BOOL	ReadConfig();
	BOOL	IsInitSQLExist();
	BOOL	LoadIniSQL();
public:
	int m_nPublicSvrID;
	int m_nLocalSvrID;

	UINT m_nDBRetryCount;

	std::string m_strGMSvrListenIPAddr;
	int m_nGMSvrListenPort;
	std::vector<int> m_vctGMSvrIPWhiteList;

	std::string m_strWarningDest;
	std::string m_strWarningPage;

	sDBConnectInfo m_sDBConnectInfo;

	char m_szModuleName[128];

	BOOL m_bIsSQLCache;
	int m_nSQLCacheCleanLine;// »º´æµÄÐ´ÈëãÚÖµ
};