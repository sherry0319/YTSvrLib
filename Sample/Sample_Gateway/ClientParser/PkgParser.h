#pragma once

#include "GameSocket.h"
#include "Socket/TCPSocket/TCPSocket.h"

void WINAPI SendRespWithError( WORD wMsgType, UINT nError, UINT nMsgSeqNo, GameSocket* pSocket, BOOL bCloseSocket=FALSE );

class CPkgParser :public YTSvrLib::ITCPSERVER, public YTSvrLib::CSingle<CPkgParser>
{
 public:   
	CPkgParser();
	virtual ~CPkgParser();

	virtual void ProcessMessage(YTSvrLib::ITCPBASE* pConn, const char* data, int len) override;

	virtual void ProcessEvent(YTSvrLib::EM_MESSAGE_TYPE emType, YTSvrLib::ITCPBASE* pConn) override;

	virtual YTSvrLib::ITCPCONNECTOR* AllocateConnector(std::string dstIP) override;

	void ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pSocket);

	virtual void SetEvent() override;

	void OnUserServerDisconnect();

	static void OnMsgRecv()
	{
		GetInstance()->MessageConsumer();
	}

	void	CheckIdleSocket( __time32_t tNow );

	void	SetClientSocket( GameSocket* pNewClientSock );
	UINT	GetNextClientID();
	GameSocket* GetClientSocket( UINT nClientID );
	void	OnClientDisconnect( GameSocket* pSocket );
	
	UINT	SendBroadCastMsg(LPCSTR szBuf,UINT nSize);

	UINT GetCurClientCount();

	sMessageTime* AllocateMessageTime(){return m_poolMessageTime.ApplyObj();}
	void ReleaseMessageTime(sMessageTime* pObj){m_poolMessageTime.ReclaimObj(pObj);}
private:

    YTSvrLib::CPool<GameSocket,128>  m_PoolPlayer;
	YTSvrLib::CPool<sMessageTime,128> m_poolMessageTime;
	
	typedef std::map<GameSocket*, __time32_t> CAcceptedClient;
	CAcceptedClient m_mapAccepted;

	UINT m_nNextClientID;
	typedef std::unordered_map<UINT, GameSocket*> ClientID2ISock;
	ClientID2ISock m_mapClientSock;
};

BOOL CheckStringValid( LPCWSTR pwzName, UINT nNameLen );

void OnClientDisconnect( GameSocket* pSocket );


