#ifndef __CONFIG_H_
#define __CONFIG_H_

class CConfig : public YTSvrLib::CSingle<CConfig>
{
public:
	CConfig(void);
	virtual ~CConfig(void);
public:
	BOOL	ReadConfig();// ∂¡»°≈‰÷√Œƒº˛
public:
	CHAR m_szReadConfigFile[MAX_PATH];
	std::vector<int> m_vctClientIPWhiteList;

	sDBConnectInfo m_sDBConnectInfo;

	UINT m_nPublicSvrID;
	UINT m_nLocalSvrID;
	std::string m_strSvrListCfg;
	UINT m_nGameID;

	std::string m_strGWListenIPAddr;
	int m_nGWListenPort;
};

#endif // !__CONFIG_H_