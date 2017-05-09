#pragma once

#include "GameSocket.h"
#include "Socket/YTSocketServer.h"

void WINAPI SendRespWithError( WORD wMsgType, UINT nError, UINT nMsgSeqNo, GameSocket* pSocket, BOOL bCloseSocket=FALSE );

class CPkgParser :public YTSvrLib::ITCPSERVER, public YTSvrLib::CPkgParserBase, public YTSvrLib::CSingle<CPkgParser>
{
 public:   
	 CPkgParser();
	 virtual ~CPkgParser();

    virtual void ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen);
    virtual void SetEvent();
    virtual void SetDisconnectEvent();

	void OnUserServerDisconnect();

	static void OnMsgRecv()
	{
		GetInstance()->OnMessageRecv();
	}

	static void OnDisconnectMsgRecv()
	{
		GetInstance()->OnDisconnectMessage();
	}
	virtual void ProcessDisconnectMsg(YTSvrLib::ITCPBASE* pSocket)
	{
		GameSocket* pGameSocket = dynamic_cast<GameSocket*>(pSocket);
		if (pGameSocket)
		{
			pGameSocket->OnDisconnect();
		}
		LOG("ProcessDisconnectMsg 0x%x", pSocket);
		m_mapAccepted.erase((GameSocket*) pSocket);
	}
	BOOL IsValidIP( const char* pszRemoteIP );

	virtual void OnServerClose()
	{

	}

	virtual void OnClosed()
	{

	}

	virtual YTSvrLib::ITCPCONNECTOR* AllocateConnector()
	{
		return m_PoolPlayer.ApplyObj();
	}

	virtual void ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pSocket)
	{
		GameSocket* pGameSocket = dynamic_cast<GameSocket*>(pSocket);
		if (pGameSocket)
		{
			if (pGameSocket->GetClientID())
			{
				m_mapClientSock.erase(pGameSocket->GetClientID());
			}

			m_mapAccepted.erase(pGameSocket);

			m_PoolPlayer.ReclaimObj(pGameSocket);
		}
	}

	virtual void ProcessAcceptedMsg(YTSvrLib::ITCPBASE* pSocket)
	{
		m_mapAccepted[(GameSocket*)pSocket] = (GET_TIME_NOW+DEFAULT_KEEP_ALIVE_EXPIRED);
		LOG("SocketBase=0x%08x Accepted.", pSocket );
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


