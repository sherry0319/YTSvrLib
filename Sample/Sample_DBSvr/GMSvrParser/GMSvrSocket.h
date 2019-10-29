#pragma once

#include "Socket/TCPSocket/TCPSocket.h"

class CGMSvrSocket : public YTSvrLib::ITCPCONNECTOR,public YTSvrLib::CRecycle
{
public:
	CGMSvrSocket(void);
	virtual ~CGMSvrSocket(void);

	virtual void Init()
	{
		YTSvrLib::ITCPBASE::Clean();
		m_bClientClosed = FALSE;
	}    

public:
	virtual int OnRecved(const char* pBuf, int nLen);

	void    PostMsg( const char* pBuf, int nLen );

	virtual void	OnClosed();

	virtual void	Send( const char* buf, int nLen );
private:
	BOOL m_bClientClosed;
};
