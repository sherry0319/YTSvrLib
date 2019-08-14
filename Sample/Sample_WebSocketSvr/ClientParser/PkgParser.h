#pragma once

#include "GameSocket.h"
#include "../Config/Config.h"

class CPkgParser :public YTSvrLib::IWSSERVER, public YTSvrLib::CSingle<CPkgParser>
{
public:
	CPkgParser();
	virtual ~CPkgParser();

	virtual void SetEvent();

	virtual void SetDisconnectEvent();

	void OnUserServerDisconnect();

	static void OnMsgRecv()
	{
		GetInstance()->onWSMsgRecv();
	}

	static void OnDisconnectMsgRecv()
	{
		GetInstance()->onWSEventRecv();
	}

	bool IsValidIP(const char* ip);

	static void SendRespWithError(WORD wMsgType, int nError, int nMsgSeqNo, GameSocket* pSocket, BOOL bCloseSocket = FALSE);

	virtual void ProcessMessage(YTSvrLib::IWSCONNECTOR* pConn, const char* msg, int len) override;

	virtual void ProcessDisconnectMsg(YTSvrLib::IWSCONNECTOR* pConn) override;

	virtual void ProcessAcceptedMsg(YTSvrLib::IWSCONNECTOR* pConn) override;

	virtual bool validateClient(std::string& dstIP) override;

	void	CheckIdleSocket(__time32_t tNow);

	void	SetClientSocket(GameSocket* pNewClientSock);
	int	GetNextClientID();
	GameSocket* GetClientSocket(UINT nClientID);
	void	OnClientDisconnect(GameSocket* pSocket);

	int	SendBroadCastMsg(LPCSTR szBuf, int nSize);

	int GetCurClientCount();

	sMessageTime* AllocateMessageTime()
	{
		return m_poolMessageTime.ApplyObj();
	}
	void ReleaseMessageTime(sMessageTime* pObj)
	{
		m_poolMessageTime.ReclaimObj(pObj);
	}
	virtual YTSvrLib::IWSCONNECTOR* AllocateConnector() override;
	virtual void ReleaseConnector(YTSvrLib::IWSCONNECTOR* pConn) override;

	char* GetSendCompressBuff()
	{
		ZeroMemory(m_pszSendCompressBuff, MAX_DATA_TO_ZIP);
		return m_pszSendCompressBuff;
	}
private:
	YTSvrLib::CPool<GameSocket, 128>  m_PoolPlayer;
	YTSvrLib::CPool<sMessageTime, 128> m_poolMessageTime;

	int m_nNextClientID;
	typedef std::unordered_map<int, GameSocket*> ClientID2ISock;
	ClientID2ISock m_mapClientSock;

	typedef std::unordered_set<GameSocket*> CAcceptedClient;
	CAcceptedClient m_mapAccepted;

	char* m_pszRecvCompressBuff;
	char* m_pszSendCompressBuff;
};
