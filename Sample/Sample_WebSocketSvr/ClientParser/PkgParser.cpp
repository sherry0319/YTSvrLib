#include "stdafx.h"
#include "PkgParser.h"

CPkgParser::CPkgParser():YTSvrLib::IWSSERVER(),m_PoolPlayer("GameSocket"), m_poolMessageTime("sMessageTime")
{
	m_nNextClientID = 1;
}
CPkgParser::~CPkgParser()
{

}

void CPkgParser::SetEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent(EAppEvent::eAppClientSocketEvent);
}

void CPkgParser::SetDisconnectEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent(EAppEvent::eAppClientSocketDisconnectEvent);
}

void CPkgParser::CheckIdleSocket(__time32_t tNow)
{
	LOG("PkgParser::CheckIdleSocket");

	std::unordered_set<void*> needRemove;

	CAcceptedClient::iterator it = m_mapAccepted.begin();
	while (it != m_mapAccepted.end())
	{
		if (it->first && it->second)
		{
			GameSocket* pSocket = it->second;
			
			if (pSocket)
			{
				__time32_t tExpired = pSocket->GetExpired();
				if (tExpired < tNow)
				{
					LOG("WEBSOCKET ====>>> PkgParser::CheckIdleSocket ctx=%x Socket=%x to Close.", it->first,pSocket);
					needRemove.insert(it->first);
				}
			}
		}
		++it;
	}

	for (auto& ctx : needRemove)
	{
		m_mapAccepted[ctx]->Close();
	}
}

bool CPkgParser::IsValidIP(const char* ip)
{
	return true;
}

void CPkgParser::SendRespWithError(WORD wMsgType, int nError, int nMsgSeqNo, GameSocket* pSocket, BOOL bCloseSocket /*= FALSE*/)
{
	sClientMsg_RespHead sResp(wMsgType);
	sResp.m_nRespRet = nError;
	sResp.m_nMsgSeqNo = nMsgSeqNo;
	pSocket->SendBinary((const char*) &sResp, sizeof(sResp));
	if (bCloseSocket)
	{
		LOG("WEBSOCKET ====>>> Socket=%x Msg=%d Ret=%d Safe Close", pSocket, wMsgType, nError);
		pSocket->Close();
	}
}

void CPkgParser::SetClientSocket(GameSocket* pNewClientSock)
{
	if (pNewClientSock->GetClientID() == 0)
	{
		pNewClientSock->SetClientID(GetNextClientID());
		m_mapClientSock[pNewClientSock->GetClientID()] = pNewClientSock;
		m_mapAccepted.erase(pNewClientSock);

		LOG("Socket=%x CID=%d", pNewClientSock, pNewClientSock->GetClientID());
	}
}

void CPkgParser::ProcessMessage(void* ctx, void* session, const char *msg, int nLen)
{
	static const int HEAD_SIZE = sizeof(sClientMsg_Head);
	YTSvrLib::IWSSESSION* s = (YTSvrLib::IWSSESSION*) session;
	GameSocket* pClientSock = m_mapAccepted[ctx];

#ifdef DEBUG64
	LOG("WEBSOCKET ====>>> ProcessMessage ctx=%x from=%x len=%d", ctx, pClientSock, nLen);
#endif

	if (pClientSock == NULL)
	{
		LOGTRACE("WEBSOCKET ====>>> ProcessMessage pClientSock NULL");
		return;
	}

	if (nLen < HEAD_SIZE)
	{
		LOGERROR("WEBSOCKET ====>>> ProcessMessage Error : SOCKET=%x CID=%d recved size less than head : %d/%d", pClientSock, pClientSock->GetClientID(), nLen, HEAD_SIZE);
		return;
	}

	sClientMsg_Head* pMsgCommHead = (sClientMsg_Head*) msg;
	if (pMsgCommHead->m_nTCPFlag != TCPFLAG_SIGN_CLIENTMSG)
	{
		// ERROR
		return;
	}

	if (nLen >= sizeof(sClientMsg_ReqHead))//正常数据包
	{
		sClientMsg_ReqHead* pReqHead = (sClientMsg_ReqHead*) pMsgCommHead;


	}
	else
	{
		LOG("WEBSOCKET ====>>> Socket=%x CID=%d Recv Less DataLen=%d", pClientSock, pClientSock->GetClientID(), nLen);
	}
}

void CPkgParser::ProcessDisconnectMsg(void* ctx, void* session)
{
	GameSocket* pSocket = m_mapAccepted[ctx];
	if (pSocket)
	{
		LOG("WEBSOCKET ====>>> ProcessDisconnectMsg: ctx=%x session=%x GameSocket=%x", ctx, session, pSocket);
		OnClientDisconnect(pSocket);
		m_mapClientSock.erase(pSocket->GetClientID());
		pSocket->OnClosed();
		m_PoolPlayer.ReclaimObj(pSocket);
	}
	else
	{
		LOG("WEBSOCKET ====>>> ProcessDisconnectMsg: ctx=%x session=%x", ctx, session);
	}
	m_mapAccepted.erase(ctx);
}

void CPkgParser::ProcessAcceptedMsg(void* ctx, void* session)
{
	YTSvrLib::IWSSESSION* s = (YTSvrLib::IWSSESSION*) session;
	GameSocket* pSocket = m_PoolPlayer.ApplyObj();
	pSocket->Create(this, (lws*)ctx, s);
	m_mapAccepted[ctx] = pSocket;
	pSocket->SetClientID(GetNextClientID());
	m_mapClientSock[pSocket->GetClientID()] = pSocket;
	pSocket->SetExpired(GET_TIME_NOW + DEFAULT_KEEP_ALIVE_EXPIRED);
	SetContextEnable(ctx, pSocket, true);
	LOG("WEBSOCKET ====>>> ProcessAcceptedMsg: ctx=%x GameSocket=%x Accepted.", ctx,pSocket);
}

bool CPkgParser::OnClientPreConnect(void* ctx, YTSvrLib::IWSSESSION* session)
{
	return IsValidIP(session->ip);
}

int CPkgParser::SendBroadCastMsg(LPCSTR szBuf, int nSize)
{
	ClientID2ISock needSend = m_mapClientSock;

	int nCount = 0;
	for (auto& pair : needSend)
	{
		GameSocket* pSocket = pair.second;
		if (pSocket)
		{
			pSocket->SendBinary(szBuf, nSize);
			nCount++;
		}
	}

	return nCount;
}

int CPkgParser::GetNextClientID()
{
	int nID = m_nNextClientID++;
	while (nID == 0 || GetClientSocket(nID) != NULL)
		nID = m_nNextClientID++;
	return nID;
}

void CPkgParser::OnClientDisconnect(GameSocket* pSocket)
{
	LOG("GameSocket=0x%x CID=%d OnClientDisconnect", pSocket, pSocket->GetClientID());
}

GameSocket* CPkgParser::GetClientSocket(UINT nClientID)
{
	ClientID2ISock::iterator it = m_mapClientSock.find(nClientID);
	if (it != m_mapClientSock.end())
	{
		GameSocket* pSocket = it->second;
		if (pSocket && pSocket->GetClientID() == nClientID)
		{
			return pSocket;
		}
	}

	return NULL;
}

int CPkgParser::GetCurClientCount()
{
	return (int) m_mapClientSock.size();
}