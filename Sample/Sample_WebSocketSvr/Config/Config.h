#pragma once

void SendGSMMessage(const char* url, const char* message);

struct sMessageTime : public YTSvrLib::CRecycle
{
	sMessageTime()
	{
		
	}

	virtual void Init()
	{
		m_nMsgType = 0;
#ifdef LIB_WINDOWS
		m_RecvTime.dwHighDateTime = 0;
		m_RecvTime.dwLowDateTime = 0;
#else
		m_tvRecv.tv_sec = 0;
		m_tvRecv.tv_usec = 0;
#endif // LIB_WINDOWS
	}

	UINT m_nMsgType;
#ifdef LIB_WINDOWS
	FILETIME m_RecvTime;
#else
	timeval m_tvRecv;
#endif // LIB_WINDOWS
};

typedef std::unordered_map<UINT, sMessageTime*> MessageRecord;

class CConfig : public YTSvrLib::CSingle<CConfig>
{
public:
	CConfig(void);
	~CConfig(void);
public:
	BOOL	ReadConfig();

	BOOL	IsCfgInited() const {return m_bCfgInited;}
public:
	std::string m_strClientListenIPAddr;
	int m_nClientListenPort;

	UINT m_nLocalSvrID;
	UINT m_nPublicSvrID;
	std::vector<int> m_vctClientIPWhiteList;

	//游戏编号
	UINT m_nGameID;

	//;客户端消息压缩
	int m_nMinClientMsgToZip;

	std::wstring m_wstrAuthTicketKey;

	std::string m_strUserSvrConnectIP;
	int m_nUserSvrListenPort;

	std::string m_strUserSvrConnectIP2;
	int m_nUserSvrListenPort2;

	std::string m_strWarningDest;
	std::string m_strWarningPage;

	std::string m_strSSLCertificateFile;
	std::string m_strSSLCertificateKeyFile;
private:
	BOOL m_bCfgInited;
};
