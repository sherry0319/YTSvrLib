#pragma once

#include "Socket/TCPSocket/TCPSocket.h"

class CGWSvrSocket : public YTSvrLib::ITCPCONNECTOR,public YTSvrLib::CRecycle
{
public:
	CGWSvrSocket(void);
	virtual ~CGWSvrSocket(void);

	virtual void Init()
	{
		YTSvrLib::ITCPBASE::Clean();
		m_nGWID = 0;
		m_bClientClosed = FALSE;
	}    

public:
	virtual int     OnRecved( const char* pBuf, int nLen );

	virtual void	OnClosed();

	virtual void    PostMsg( const char* pBuf, int nLen );

	void SetGWID( UINT nID ) { m_nGWID = nID; }
	UINT GetGWID() { return m_nGWID; }
private:
	UINT m_nGWID;
	BOOL m_bClientClosed;
};
