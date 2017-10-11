#include "stdafx.h"
#include "PkgParser.h"

void WINAPI SendRespWithError( WORD wMsgType, UINT nError, UINT nMsgSeqNo, GameSocket* pSocket, BOOL bCloseSocket/*=FALSE */ )
{
	sClientMsg_RespHead sResp(wMsgType);
	sResp.m_nRespRet = nError;
	sResp.m_nMsgSeqNo = nMsgSeqNo;
	pSocket->Send( (char*)&sResp, sizeof(sResp) );
	if( bCloseSocket )
	{
		LOG("Socket=%d Msg=0x%04x Ret=%d close", pSocket->GetSocket(), wMsgType, nError );
		pSocket->SafeClose();
	}
	return;
}
void OnClientDisconnect( GameSocket* pSocket )
{
	CPkgParser::GetInstance()->OnClientDisconnect(pSocket);
}

//////////////////////////////////////////////////////////////////////////
CPkgParser::CPkgParser()
:YTSvrLib::CPkgParserBase(), m_PoolPlayer("GameSocket"),m_poolMessageTime("sMessageTime")
{ 
	m_nNextClientID = 1;
}
CPkgParser::~CPkgParser()
{

}

extern time_t gTime;


void CPkgParser::SetEvent()
{
    YTSvrLib::CServerApplication::GetInstance()->SetEvent( EAppEvent::eAppClientSocketEvent );
}

void CPkgParser::SetDisconnectEvent()
{
    YTSvrLib::CServerApplication::GetInstance()->SetEvent( EAppEvent::eAppClientSocketDisconnectEvent );
}

BOOL CPkgParser::IsValidIP( const char* pszRemoteIP )
{
	int nRemoteAddr = inet_addr( pszRemoteIP );
	for( UINT i=0; i< CConfig::GetInstance()->m_vctClientIPWhiteList.size(); i++ )
	{
		if( CConfig::GetInstance()->m_vctClientIPWhiteList[i] == 0 )
			return TRUE;
		if( CConfig::GetInstance()->m_vctClientIPWhiteList[i] == nRemoteAddr )
			return TRUE;
	}
	LOG("Client _IsValidIP Invalid RemoteIP=%s Error!", pszRemoteIP );
	return FALSE;
}

void CPkgParser::CheckIdleSocket( __time32_t tNow )
{
	LOG("PkgParser::CheckIdleSocket");

	vector<GameSocket*> vctNeedRemove;

	CAcceptedClient::iterator it = m_mapAccepted.begin();
	while (it != m_mapAccepted.end())
	{
		if (it->first && it->second)
		{
			GameSocket* pSocket = it->first;
			__time32_t tExpired = it->second;
			if (pSocket && tExpired < tNow)
			{
				m_mapClientSock.erase(pSocket->GetClientID());
				++it;
				LOG("PkgParser::CheckIdleSocket Socket=%d to Close.", pSocket->GetSocket());
				pSocket->OnDisconnect();
				vctNeedRemove.push_back(pSocket);
				continue;
			}
		}
		++it;
	}

	for (size_t i = 0; i < vctNeedRemove.size();++i)
	{
		m_mapAccepted.erase(vctNeedRemove[i]);
	}
}
void CPkgParser::ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen)
{
	GameSocket* pClientSock = dynamic_cast<GameSocket*>(pSocket);
	LOG("Recv Client Message From IP=%s CurQueue=%d",pClientSock->GetAddrIp(),GetInstance()->m_qMsg.size());
	if( nLen < sizeof(sClientMsg_Head) )
	{
		LOG("Socket=%d CID=%d Recv Less DataLen=%d<%d", pClientSock->GetSocket(), pClientSock->GetClientID(), nLen, sizeof(sClientMsg_Head) );
		return;
	}
	sClientMsg_Head* pMsgCommHead = (sClientMsg_Head*)pBuf;

	BYTE szUnZippedBuf[MAX_DATA_TO_ZIP+1024];
	if( pMsgCommHead->m_nZipEncrypFlag&_CLIENTMSG_FLAG_ZIPPED
		/*&& GetInstance()->m_pfnZUncompress != NULL*/ && pMsgCommHead->m_nZipSrcLen < MAX_DATA_TO_ZIP )
	{
		memcpy( szUnZippedBuf, pMsgCommHead, sizeof(sClientMsg_Head) );
		ULONG nUnZippedBufLen = sizeof(szUnZippedBuf) - sizeof(sClientMsg_Head);
		
		int nRet = uncompress(szUnZippedBuf + sizeof(sClientMsg_Head), &nUnZippedBufLen, (BYTE*) (pMsgCommHead + 1), nLen - sizeof(sClientMsg_Head));
		if( nRet == Z_OK )
		{
			pMsgCommHead = (sClientMsg_Head*)szUnZippedBuf;
			pMsgCommHead->m_nMsgLenTotal = sizeof(sClientMsg_Head)+nUnZippedBufLen;
			nLen = pMsgCommHead->m_nMsgLenTotal;
		}
		else
		{
			LOG("Socket=%d CID=%d Msg=0x%04x UnZip Error=%d", pClientSock->GetSocket(), pClientSock->GetClientID(), pMsgCommHead->m_nMsgType, nRet);
			return;
		}
	}
	if( nLen >= sizeof(sClientMsg_ReqHead) )//正常数据包
	{
		sClientMsg_ReqHead* pReqHead = (sClientMsg_ReqHead*)pMsgCommHead;
		pReqHead->CheckData( nLen );

		m_mapAccepted[pClientSock] = (GET_TIME_NOW + DEFAULT_KEEP_ALIVE_EXPIRED);

		SetClientSocket( pClientSock );
		LPCSTR pszMsgBody = (LPCSTR)(pReqHead+1);
		int nBodyLen = nLen-sizeof(sClientMsg_ReqHead);

		if( pReqHead->m_nMsgType > 0 )
		{
			LOG("Socket=%d CID=%d User=%d Recv Msg=0x%04x Len=%d From=%s:%d", 
					pClientSock->GetSocket(), pClientSock->GetClientID(), 
					pReqHead->m_nUserID, pReqHead->m_nMsgType, nBodyLen,
					pClientSock->GetAddrIp(), pClientSock->GetAddrPort() );
		}

		if( pReqHead->m_nMsgType >= 0x0200)	// Forward to different server by message type
		{
			CServerSocket* pSvrSocket = CServerParser::GetInstance()->GetSvrSocket(emAgent_UserSvr);
			
			if( pSvrSocket == NULL || pSvrSocket->IsConnectedSvr() == false )
			{
				SendRespWithError(RESP(pReqHead->m_nMsgType), RET_USERSVR1_NOT_START, pReqHead->m_nMsgSeqNo, (GameSocket*) pSocket, FALSE);
				return;
			}
			pClientSock->OnRecvNewMsg(pReqHead->m_nMsgSeqNo,pReqHead->m_nMsgType);
			pSvrSocket->SendMsgToSvr(emAgent_Client, pClientSock->GetAddrIp(), pClientSock->GetClientID(), (LPCSTR) pMsgCommHead, nLen);
		}
		else
		{
			CServerSocket* pSvrSocket = CServerParser::GetInstance()->GetSvrSocket(emAgent_UserSvr2);

			if (pSvrSocket == NULL || pSvrSocket->IsConnectedSvr() == false)
			{
				SendRespWithError(RESP(pReqHead->m_nMsgType), RET_USERSVR2_NOT_START, pReqHead->m_nMsgSeqNo, (GameSocket*) pSocket, FALSE);
				return;
			}
			pClientSock->OnRecvNewMsg(pReqHead->m_nMsgSeqNo, pReqHead->m_nMsgType);
			pSvrSocket->SendMsgToSvr(emAgent_Client, pClientSock->GetAddrIp(), pClientSock->GetClientID(), (LPCSTR) pMsgCommHead, nLen);
		}
	}
	else
		LOG("Socket=%d CID=%d Recv Less DataLen=%d", pClientSock->GetSocket(), pClientSock->GetClientID(), nLen );
}

void CPkgParser::SetClientSocket( GameSocket* pNewClientSock )
{
	if( pNewClientSock->GetClientID() == 0 )
	{
		pNewClientSock->SetClientID( GetNextClientID() );
		m_mapClientSock[pNewClientSock->GetClientID()] = pNewClientSock;
		m_mapAccepted.erase( pNewClientSock );

		LOG("Socket=%d CID=%d", pNewClientSock->GetSocket(), pNewClientSock->GetClientID() );
	}
}

UINT CPkgParser::SendBroadCastMsg(LPCSTR szBuf,UINT nSize)
{
	UINT nCount = 0;
	ClientID2ISock::iterator it = m_mapClientSock.begin();
	while (it != m_mapClientSock.end())
	{
		GameSocket* pSocket = it->second;
		if (pSocket)
		{
			pSocket->Send(szBuf,nSize);
			nCount++;
		}
		++it;
	}

	return nCount;
}

UINT CPkgParser::GetNextClientID()
{
	UINT nID = m_nNextClientID++;
	while( nID == 0 || GetClientSocket( nID ) != NULL )
		nID = m_nNextClientID++;
	return nID;
}

void CPkgParser::OnClientDisconnect( GameSocket* pSocket )
{
	LOG("Socket=%d CID=%d OnClientDisconnect", pSocket->GetSocket(), pSocket->GetClientID() );
	if( pSocket->GetClientID() != 0 )
		m_mapClientSock.erase( pSocket->GetClientID() );

	m_mapAccepted.erase( pSocket );

	CServerSocket* pSvrSock = CServerParser::GetInstance()->GetUserSvrSocket( );

	CServerSocket* pSvrSock2 = CServerParser::GetInstance()->GetUserSvrSocket2();

	sGWMsg_ClientDisconnect sGWMsg;
	sGWMsg.m_From.m_emType = emAgent_GateWay;
	sGWMsg.m_From.m_nAgentID = pSocket->GetClientID();
	sGWMsg.m_To.m_emType = emAgent_UserSvr;
	sGWMsg.m_To.m_nAgentID = pSvrSock->GetSvrID();
	strncpy_s(sGWMsg.m_szClientIP, pSocket->GetAddrIp(), 31);
	sGWMsg.m_nSvrID = CConfig::GetInstance()->m_nPublicSvrID;
	sGWMsg.m_nGWID = CConfig::GetInstance()->m_nLocalSvrID;
	sGWMsg.m_nClientID = pSocket->GetClientID();

	if( pSvrSock && pSvrSock->IsConnectedSvr() == TRUE )
	{
		pSvrSock->Send((LPCSTR)(&sGWMsg),sizeof(sGWMsg));
	}
	if (pSvrSock2 && pSvrSock2->IsConnectedSvr() == TRUE)
	{
		sGWMsg.m_To.m_emType = emAgent_UserSvr2;
		sGWMsg.m_To.m_nAgentID = pSvrSock2->GetSvrID();
		pSvrSock2->Send((LPCSTR) (&sGWMsg), sizeof(sGWMsg));
	}
}

GameSocket* CPkgParser::GetClientSocket( UINT nClientID )
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
	vector<GameSocket*> vctDisconnect;
	ClientID2ISock::iterator it = m_mapClientSock.begin();
	while (it != m_mapClientSock.end())
	{
		GameSocket* pSocket = it->second;
		if (pSocket)
		{
			vctDisconnect.push_back(pSocket);
		}

		++it;
	}

	m_mapClientSock.clear();
	m_mapAccepted.clear();

	for (size_t i = 0; i < vctDisconnect.size();++i)
	{
		vctDisconnect[i]->SafeClose();
	}

	vctDisconnect.clear();
}

UINT CPkgParser::GetCurClientCount()
{
	return (UINT)m_mapClientSock.size();
}