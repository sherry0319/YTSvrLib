#pragma once

#include "Socket/YTSocketConnector.h"

class CGMSvrSocket : public YTSvrLib::ITCPCONNECTOR,public YTSvrLib::CRecycle
{
public:
	CGMSvrSocket(void);
	virtual ~CGMSvrSocket(void);

	virtual void Init()
	{
		YTSvrLib::ITCPCONNECTOR::Clean();
		m_bClientClosed = FALSE;
	}    

public:
	virtual int     OnRecved(const char* pBuf, int nLen);

	void    PostMsg( const char* pBuf, int nLen );

	void    PostDisconnectMsg( EType eType );    

	virtual void OnDisconnect();
	virtual void OnClosed();

	virtual void	Send( const char* buf, int nLen );
	virtual void    Send(  const std::string& strPkg );  
	virtual void    Send(  const std::string* pStrPkg );  

	virtual void    ReclaimObj();
private:
	BOOL m_bClientClosed;
};
