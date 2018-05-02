#pragma once

#include "GameSocket.h"

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

	virtual void ProcessMessage(void* ctx, void* session, const char *msg, int len);

	virtual void ProcessDisconnectMsg(void* ctx, void* session);

	virtual void ProcessAcceptedMsg(void* ctx, void* session);

	virtual bool OnClientPreConnect(void* ctx, YTSvrLib::IWSSESSION* session) override;

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
private:
	YTSvrLib::CPool<GameSocket, 128>  m_PoolPlayer;
	YTSvrLib::CPool<sMessageTime, 128> m_poolMessageTime;

	typedef std::unordered_map<void*, GameSocket*> CAcceptedClient;
	CAcceptedClient m_mapAccepted;

	int m_nNextClientID;
	typedef std::unordered_map<int, GameSocket*> ClientID2ISock;
	ClientID2ISock m_mapClientSock;
};

BOOL CheckStringValid(LPCWSTR pwzName, int nNameLen);

void DecryptMACInfo(wchar_t* buf, int nLen, int nKey);


