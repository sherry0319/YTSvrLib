#include "stdafx.h"
#include "PkgParser.h"
#include "../../Common/ClientDef.h"
#include "../timer/TimerMgr.h"
#include "../Global.h"
#include "../../Common/MsgRetDef.h"
#include "../ServerParser/ServerSocket.h"
#include "../ServerParser/ServerParser.h"

//////////////////////////////////////////////////////////////////////////
CPkgParser::CPkgParser() :YTSvrLib::IWSSERVER(), m_PoolPlayer("GameSocket"), m_poolMessageTime("sMessageTime")
{
	m_nNextClientID = 1;
	m_pszRecvCompressBuff = new char[MAX_DATA_TO_ZIP];
	m_pszSendCompressBuff = new char[MAX_DATA_TO_ZIP];
}
CPkgParser::~CPkgParser()
{
	if (m_pszRecvCompressBuff)
	{
		delete[] m_pszRecvCompressBuff;
		m_pszRecvCompressBuff = NULL;
	}
	if (m_pszSendCompressBuff)
	{
		delete[] m_pszSendCompressBuff;
		m_pszSendCompressBuff = NULL;
	}
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

	CAcceptedClient needRemove;

	for (auto& pSocket : m_mapAccepted)
	{
		if (pSocket)
		{
			__time32_t tExpired = pSocket->GetExpired();
			if (tExpired < tNow)
			{
				LOG("WEBSOCKET ====>>> PkgParser::CheckIdleSocket Socket=%x CID=%d Expired=%ld to Close.", pSocket, pSocket->GetClientID(), tExpired);
				needRemove.insert(pSocket);
			}
		}
	}

	for (auto& ctx : needRemove)
	{
		ctx->Close();
	}
}

bool CPkgParser::IsValidIP(const char* ip)
{
	// 校验ip是否允许连接

	return true;
}

void CPkgParser::SendRespWithError(WORD wMsgType, int nError, int nMsgSeqNo, GameSocket* pSocket, BOOL bCloseSocket /*= FALSE*/)
{
	sClientMsg_RespHead sResp(wMsgType);
	sResp.m_nRespRet = nError;
	sResp.m_nMsgSeqNo = nMsgSeqNo;
	pSocket->SendBinary((const char*)& sResp, sizeof(sResp));
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

void CPkgParser::ProcessMessage(YTSvrLib::IWSCONNECTOR* pConn, const char* msg, int nLen)
{
	static const int HEAD_SIZE = sizeof(sClientMsg_Head);

	GameSocket* pClientSock = dynamic_cast<GameSocket*>(pConn);

#ifdef DEBUG64
	LOG("WEBSOCKET ====>>> ProcessMessage from=%x len=%d", pClientSock, nLen);
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

	sClientMsg_Head* pMsgCommHead = (sClientMsg_Head*)msg;
	if (pMsgCommHead->m_nTCPFlag != TCPFLAG_SIGN_CLIENTMSG)
	{
		pClientSock->SetExpired(GET_TIME_NOW + HANGON_KEEP_ALIVE_EXPIRED);
		SendRespWithError(RESP(pMsgCommHead->m_nMsgType), RET_FAIL, pMsgCommHead->m_nMsgSeqNo, pClientSock, TRUE);
		return;
	}
	ZeroMemory(m_pszRecvCompressBuff, MAX_DATA_TO_ZIP);
	if (pMsgCommHead->m_nZipEncrypFlag & _CLIENTMSG_FLAG_ZIPPED
		/*&& GetInstance()->m_pfnZUncompress != NULL*/ && pMsgCommHead->m_nZipSrcLen < MAX_DATA_TO_ZIP)
	{
		memcpy(m_pszRecvCompressBuff, pMsgCommHead, sizeof(sClientMsg_Head));
		ULONG nUnZippedBufLen = MAX_DATA_TO_ZIP - sizeof(sClientMsg_Head);

#ifdef ZIP_LZ4
		nUnZippedBufLen = YTSvrLib::LZ4FEasy::LZ4F_decompress((const char*)(pMsgCommHead + 1), (char*)(m_pszRecvCompressBuff + sizeof(sClientMsg_Head)), nLen - sizeof(sClientMsg_Head), MAX_DATA_TO_ZIP);
		if (nUnZippedBufLen > 0)
#else
		int nRet = uncompress((Bytef*)(m_pszRecvCompressBuff + sizeof(sClientMsg_Head)), &nUnZippedBufLen, (BYTE*)(pMsgCommHead + 1), nLen - sizeof(sClientMsg_Head));
		if (nRet == Z_OK)
#endif
		{
			pMsgCommHead = (sClientMsg_Head*)m_pszRecvCompressBuff;
			pMsgCommHead->m_nMsgLenTotal = sizeof(sClientMsg_Head) + nUnZippedBufLen;
			nLen = pMsgCommHead->m_nMsgLenTotal;
		}
		else
		{
#ifdef ZIP_LZ4
			LOG("Socket=%x CID=%d Msg=%d LZ4 decompress Error=%d", pClientSock, pClientSock->GetClientID(), pMsgCommHead->m_nMsgType, nUnZippedBufLen);
#else
			LOG("Socket=%x CID=%d Msg=%d UnZip Error=%d", pClientSock, pClientSock->GetClientID(), pMsgCommHead->m_nMsgType, nRet);
#endif
			return;
		}
#ifdef _DEBUG_LOG
		LOG("Socket=%x CID=%d UnZip Msg=%d OK", pClientSock, pClientSock->GetClientID(), pMsgCommHead->m_nMsgType);
#endif
	}
	if (nLen >= sizeof(sClientMsg_ReqHead))// 正常数据包
	{
		sClientMsg_ReqHead* pReqHead = (sClientMsg_ReqHead*)pMsgCommHead;
		pReqHead->CheckData(nLen);

		pClientSock->SetExpired(GET_TIME_NOW + DEFAULT_KEEP_ALIVE_EXPIRED);

		SetClientSocket(pClientSock);
		LPCSTR pszMsgBody = (LPCSTR)(pReqHead + 1);
		int nBodyLen = nLen - sizeof(sClientMsg_ReqHead);

		if (pReqHead->m_nMsgType > 0)
		{
			LOG("WEBSOCKET ====>>> Socket=%x CID=%d User=%d Recv Msg=%d Len=%d From=%s:%d",
				pClientSock, pClientSock->GetClientID(),
				pReqHead->m_nUserID, pReqHead->m_nMsgType, nBodyLen,
				pClientSock->GetAddrIp(), pClientSock->GetAddrPort());
		}

		LOG("Handle Normal Client Req : Seqno=%d Type=%d Len=%d",pReqHead->m_nMsgSeqNo,pReqHead->m_nMsgType,pReqHead->m_nMsgLenTotal);
	}
	else
		LOG("WEBSOCKET ====>>> Socket=%x CID=%d Recv Less DataLen=%d", pClientSock, pClientSock->GetClientID(), nLen);
}

void CPkgParser::ProcessDisconnectMsg(YTSvrLib::IWSCONNECTOR* pConn)
{
	GameSocket* pSocket = (GameSocket*)pConn;
	LOG("WEBSOCKET ====>>> ProcessDisconnectMsg: GameSocket=%x", pSocket);
	if (pSocket)
	{
		OnClientDisconnect(pSocket);
		m_mapClientSock.erase(pSocket->GetClientID());
		pSocket->OnClosed();
	}
	m_mapAccepted.erase(pSocket);
	ReleaseConnector(pSocket);
}

void CPkgParser::ProcessAcceptedMsg(YTSvrLib::IWSCONNECTOR* pConn)
{
	GameSocket* pSocket = (GameSocket*)pConn;
	pSocket->InitData();
	m_mapAccepted.insert(pSocket);
	pSocket->SetClientID(GetNextClientID());
	m_mapClientSock[pSocket->GetClientID()] = pSocket;
	pSocket->SetExpired(GET_TIME_NOW + DEFAULT_KEEP_ALIVE_EXPIRED);
	LOG("WEBSOCKET ====>>> ProcessAcceptedMsg: GameSocket=%x CID=%d Accepted.", pSocket, pSocket->GetClientID());
}

bool CPkgParser::validateClient(std::string& dstIP)
{
	return IsValidIP(dstIP.c_str());
}

YTSvrLib::IWSCONNECTOR* CPkgParser::AllocateConnector() {
	return m_PoolPlayer.ApplyObj();
}

void CPkgParser::ReleaseConnector(YTSvrLib::IWSCONNECTOR* pConn) {
	m_PoolPlayer.ReclaimObj((GameSocket*)pConn);
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

	CServerSocket* pSvrSock = CServerParser::GetInstance()->GetUserSvrSocket();

	sGWMsg_ClientDisconnect sGWMsg;
	sGWMsg.m_From.m_emType = emAgent_GateWay;
	sGWMsg.m_From.m_nAgentID = pSocket->GetClientID();
	sGWMsg.m_To.m_emType = emAgent_UserSvr;
	sGWMsg.m_To.m_nAgentID = pSvrSock->GetSvrID();
	strncpy_s(sGWMsg.m_szClientIP, pSocket->GetAddrIp(), 31);
	sGWMsg.m_nSvrID = CConfig::GetInstance()->m_nPublicSvrID;
	sGWMsg.m_nGWID = CConfig::GetInstance()->m_nLocalSvrID;
	sGWMsg.m_nClientID = pSocket->GetClientID();

	if (pSvrSock && pSvrSock->IsConnectedSvr() == TRUE)
	{
		pSvrSock->Send((LPCSTR)(&sGWMsg), sizeof(sGWMsg));
	}
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

void CPkgParser::OnUserServerDisconnect()
{
	LOG("OnUserServerDisconnect");
	std::unordered_set<GameSocket*> needClose;
	ClientID2ISock::iterator it = m_mapClientSock.begin();
	while (it != m_mapClientSock.end())
	{
		GameSocket* pSocket = it->second;
		if (pSocket)
		{
			needClose.insert(pSocket);
		}

		++it;
	}

	for (auto& gs : needClose)
	{
		gs->Close();
	}

	needClose.clear();
}

int CPkgParser::GetCurClientCount()
{
	return (int)m_mapClientSock.size();
}