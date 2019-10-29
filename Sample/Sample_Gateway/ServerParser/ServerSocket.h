#pragma once
#include "GWMsgDef.h"
#include "Socket/TCPSocket/TCPSocket.h"

#define MAX_CONNECTING_RETRY	10

class CServerSocket : public YTSvrLib::ITCPCLIENT,public YTSvrLib::CRecycle
{
public:
	CServerSocket(void);
	virtual ~CServerSocket(void);

	virtual void Init();

	virtual int     OnRecved(const char* pBuf, int nLen);

	virtual void	OnClosed();

	virtual void    PostMsg( const char* pBuf, int nLen );

	void SetSvrInfo( EM_AGENT emSvrType, int nSvrID, LPCSTR destHost,int nPort);
	EM_AGENT GetSvrType() { return m_emSvrType; }
	int GetSvrID() { return m_nSvrID; } 

	BOOL ConnectToSvr();

	virtual void OnConnected();

	void OnConnectFailed();

	BOOL	IsConnectedSvr() { return m_bConnectedSvr; }

	void SendKeepAlive();

	void SendMsgToSvr( EM_AGENT emFromAgtType,const char* pszClientIP, UINT nFromAgtID, LPCSTR pszMsg, int nMsgLen );
	UINT m_nUserCnt;
private:
	BOOL m_bConnectedSvr;
	int m_nConnectingTimes;

	EM_AGENT m_emSvrType;
	int m_nSvrID;
	char m_szSvrIPAddr[16];
	int m_nSvrTcpPort;

	
};
typedef std::list<CServerSocket*> ListPSvrSocket;
