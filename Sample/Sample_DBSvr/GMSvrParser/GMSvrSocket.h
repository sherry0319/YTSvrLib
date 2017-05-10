#pragma once

#include "Socket/YTSocketConnector.h"

class CGMSvrSocket : public YTSvrLib::ITCPCONNECTOR,public YTSvrLib::CRecycle
{
public:
	CGMSvrSocket(void);
	virtual ~CGMSvrSocket(void);

	DWORD m_nRecvSeqNo;
	DWORD m_nSendSeqNo;
	virtual void Init()
	{
		YTSvrLib::ITCPCONNECTOR::Clean();
		m_nRecvSeqNo = 0;
		m_nSendSeqNo = 0;
	}    

public:
	virtual int     OnRecved(const char* pBuf, int nLen);

	void    PostMsg( const char* pBuf, int nLen );

	void    PostDisconnectMsg( EType eType );    

	virtual void OnClosed();

	virtual void	Send( const char* buf, int nLen );
	virtual void    Send(  const std::string& strPkg );  
	virtual void    Send(  const std::string* pStrPkg );  

	virtual void    ReclaimObj();
};
