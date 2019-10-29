#pragma once
#include  <algorithm>
#include "Socket/TCPSocket/TCPSocket.h"

class GameSocket : public YTSvrLib::ITCPCONNECTOR, public YTSvrLib::CRecycle
{
	__time32_t m_tIdleExpired;	
public:
	GameSocket():ITCPCONNECTOR()
	{
		m_sendBuf.SetQueueLenMax( 1024 );
	}
    virtual void Init();

public:
	// virtual int		OnSocketRecv();
    virtual int     OnRecved( const char* pBuf, int nLen ) override;

	virtual void	OnClosed();

    virtual void    PostMsg( const char* pBuf, int nLen );

    virtual void    Send( const char* buf, int nLen );

	void	SetClientID( UINT nID ) { m_nClientID = nID; }
	UINT	GetClientID() { return m_nClientID; }

	void	OnRecvNewMsg(UINT nMsgSeqno,UINT nMsgType);
	void	OnSendMsg(UINT nMsgSeqno,UINT nMsgType);
private:
	void ReclaimObj();
private:
	UINT m_nClientID;
	MessageRecord m_mapMessageRecved;
	BOOL m_bClientClosed;
};