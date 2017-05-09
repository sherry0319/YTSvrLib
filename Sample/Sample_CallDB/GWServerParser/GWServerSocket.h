#pragma once

#define GWMSG_BUFSIZE	2048
struct sGWMsgBuf : public YTSvrLib::CRecycle
{
	char m_szBuf[GWMSG_BUFSIZE];
	int m_nLen;

	virtual void Init()
	{
		m_nLen = 0;
	}
};

#include "Socket/YTSocketConnector.h"

class CGWSvrSocket : public YTSvrLib::ITCPCONNECTOR,public YTSvrLib::CRecycle
{
public:
	CGWSvrSocket(void);
	virtual ~CGWSvrSocket(void);

	DWORD m_nRecvSeqNo;
	DWORD m_nSendSeqNo;
	virtual void Init()
	{
		YTSvrLib::ITCPCONNECTOR::Clean();
		m_nRecvSeqNo = 0;
		m_nSendSeqNo = 0;
		m_nGWID = 0;
	}    

public:
	virtual int		OnSocketRecv();
	virtual int     OnRecved( const char* pBuf, int nLen );

	virtual void	OnClosed();
	virtual void    PostMsg( const char* pBuf, int nLen );
	virtual void    PostDisconnectMsg( EType eType ); 

	virtual void    ReclaimObj();

	void SetGWID( UINT nID ) { m_nGWID = nID; }
	UINT GetGWID() { return m_nGWID; }
private:
	UINT m_nGWID;
};
