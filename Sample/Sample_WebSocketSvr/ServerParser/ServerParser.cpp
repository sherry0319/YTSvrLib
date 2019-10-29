#include "stdafx.h"
#include "ServerParser.h"

CServerParser::CServerParser(void) : m_PoolSvrSocket("CServerSocket")
{
	//m_pAccSvrSocket = NULL;
	m_pUserSocket = NULL;
	m_pUserSocket2 = NULL;
	m_tLastSendGSM = GET_TIME_NOW-(29*SEC_MINUTE);
	m_tLastSendGSMUser = GET_TIME_NOW;
}

CServerParser::~CServerParser(void)
{
}


void CServerParser::SetEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent( EAppEvent::eAppServerSocketEvent );
}

void CServerParser::OnSvrDisconnect( CServerSocket* pSocket )
{

}

CServerSocket* CServerParser::GetSvrSocket( EM_AGENT emSvrType)
{
	switch( emSvrType )
	{
		case emAgent_UserSvr:
		{
			return m_pUserSocket;
		}
		break;
		case emAgent_UserSvr2:
		{
			return m_pUserSocket2;
		}
		break;
	default:
		return NULL;
	}
	return NULL;
}
CServerSocket* CServerParser::InitUserSvrSocket()
{
	if( m_pUserSocket == NULL )
	{
		m_pUserSocket = m_PoolSvrSocket.ApplyObj();
		if( m_pUserSocket == NULL )
		{
			LOG("UserSvr InitUserSvrSocket ApplyObj Error!");
			return NULL;
		}
	}
	m_pUserSocket->SetSvrInfo( emAgent_UserSvr, 1, CConfig::GetInstance()->m_strUserSvrConnectIP.c_str(), CConfig::GetInstance()->m_nUserSvrListenPort );

	m_pUserSocket->ConnectToSvr();

	return m_pUserSocket;
}

CServerSocket* CServerParser::InitUserSvrSocket2()
{
	if (m_pUserSocket2 == NULL)
	{
		m_pUserSocket2 = m_PoolSvrSocket.ApplyObj();
		if (m_pUserSocket2 == NULL)
		{
			LOG("UserSvr 2 InitUserSvrSocket ApplyObj Error!");
			return NULL;
		}
	}
	m_pUserSocket2->SetSvrInfo(emAgent_UserSvr2, 1, CConfig::GetInstance()->m_strUserSvrConnectIP2.c_str(), CConfig::GetInstance()->m_nUserSvrListenPort2);

	m_pUserSocket2->ConnectToSvr();

	return m_pUserSocket2;
}

void CServerParser::ProcessEvent(YTSvrLib::EM_MESSAGE_TYPE emType, YTSvrLib::ITCPBASE* pConn) {
	LOG("ProcessEvent : type=[%d] pConn=[%x]", emType, pConn);
	CServerSocket* pSvrSocket = dynamic_cast<CServerSocket*>(pConn);
	switch (emType)
	{
	case YTSvrLib::MSGTYPE_DISCONNECT: {
		pSvrSocket->OnClosed();
	}break;
	case YTSvrLib::MSGTYPE_CONNECTED: {
		pSvrSocket->OnConnected();
	}break;
	case YTSvrLib::MSGTYPE_CONNECTFAILED: {
		pSvrSocket->OnConnectFailed();
	}break;
	default:
		break;
	}
}

void CServerParser::ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen)
{
	CServerSocket* pSvrSock = dynamic_cast<CServerSocket*>(pSocket);
	if (pSvrSock == NULL)
	{
		LOG("Invalid Server Socket = 0x%x",pSocket);
		return;
	}
	if( nLen < sizeof( sGWMsg_Head) )
	{
		LOG("Svr=%d Socket=%d Recv Invalid DataLen=%d Error!", pSvrSock->GetSvrID(), pSvrSock->GetSocket(), nLen );
		return;
	}
	sGWMsg_Head* pSvrMsgHead = (sGWMsg_Head*)pBuf;

	switch( pSvrMsgHead->m_nMsgType )
	{
	case T_GWMSG_KEEPALIVE:
		{
			sGWMsg_KeepAlive* pMsgKeepAlive = (sGWMsg_KeepAlive*)pBuf;
			switch( pSvrSock->GetSvrType() )
			{
			case emAgent_UserSvr:
				{
					pSvrSock->m_nUserCnt = pMsgKeepAlive->m_nRunParam;
					LOG("UserSvr=%d ActiveUser=%d", pSvrSock->GetSvrID(), pSvrSock->m_nUserCnt );
					break;
				}
			}
			break;
		}
	case T_GWMSG_SVRLOGIN:
		break;
	case T_GWMSG_C2S://
		break;
	case T_GWMSG_S2C://
		{
			GameSocket* pClientSock = CPkgParser::GetInstance()->GetClientSocket( pSvrMsgHead->m_To.m_nAgentID );
			sClientMsg_RespHead* pClientMsg = (sClientMsg_RespHead*)(pSvrMsgHead+1);
			if( pClientSock )
			{
				if (pClientMsg->m_nMsgType >= 0x2001)
				{
					LOG("Send Client Notify CID=%d Msg=0x%04x nLen=%d Ret=%d",pClientSock->GetClientID(),pClientMsg->m_nMsgType,pClientMsg->m_nMsgLenTotal,pClientMsg->m_nRespRet);
				}
				else
				{
					pClientSock->OnSendMsg(pClientMsg->m_nMsgSeqNo,pClientMsg->m_nMsgType);
					LOG("Send Client Resp CID=%d Msg=0x%04x nLen=%d Ret=%d",pClientSock->GetClientID(),pClientMsg->m_nMsgType,pClientMsg->m_nMsgLenTotal,pClientMsg->m_nRespRet);
				}

				pClientSock->Send( (LPCSTR)(pSvrMsgHead+1), nLen - sizeof(sGWMsg_Head) );
			}
			else
			{
				LOG("Error On Send Client Msg=0x%04x nLen=%d CID=%d",pClientMsg->m_nMsgType,pClientMsg->m_nMsgLenTotal,pSvrMsgHead->m_To.m_nAgentID);
			}
			break;
		}
		break;
	case T_GWMSG_CLIENTBROADCAST:
		{
			sClientMsg_RespHead* pClientMsg = (sClientMsg_RespHead*)(pSvrMsgHead+1);

			UINT nCount = CPkgParser::GetInstance()->SendBroadCastMsg((LPCSTR)(pSvrMsgHead+1), nLen - sizeof(sGWMsg_Head));

			LOG("Send Client Broadcast Msg=0x%04x nLen=%d Ret=%d Succ=%d",pClientMsg->m_nMsgType,pClientMsg->m_nMsgLenTotal,pClientMsg->m_nRespRet,nCount);
		}
		break;
	case T_GWMSG_S2S:
		{
			CServerSocket* pSendSvrSock = GetSvrSocket( pSvrMsgHead->m_To.m_emType);
			if( pSendSvrSock == NULL )
			{
				LOG("Svr=%d Socket=%d Recv Invalid DstSvr=%d Type=%d Error!", pSvrSock->GetSvrID(), pSvrSock->GetSocket(), pSvrMsgHead->m_To.m_nAgentID, pSvrMsgHead->m_To.m_emType );
				pSvrMsgHead->m_nMsgType = T_GWMSG_S2S_ERR;
				pSvrSock->Send( pBuf, nLen );
				break;
			}
			if( pSendSvrSock->IsConnectedSvr() == FALSE )
			{
				LOG("Svr=%d Socket=%d DstSvr=%d Type=%d Disconnected Error!", pSvrSock->GetSvrID(), pSvrSock->GetSocket(), pSvrMsgHead->m_To.m_nAgentID, pSvrMsgHead->m_To.m_emType );
				pSvrMsgHead->m_nMsgType = T_GWMSG_S2S_ERR;
				pSvrSock->Send( pBuf, nLen );
				break;
			}
			pSvrMsgHead->m_RouteBy.m_emType = emAgent_GateWay;
			pSvrMsgHead->m_RouteBy.m_nAgentID = CConfig::GetInstance()->m_nLocalSvrID;
			pSendSvrSock->Send( pBuf, nLen );

			LOG("Svr=%d Socket=%d recv Msg=0x%04x Len=%d From=%d_%d To=%d_%d",
				pSvrSock->GetSvrID(), pSvrSock->GetSocket(), 
				pSvrMsgHead->m_nMsgType, pSvrMsgHead->m_nTotalMsgLen,
				pSvrMsgHead->m_From.m_emType, pSvrMsgHead->m_From.m_nAgentID,
				pSvrMsgHead->m_To.m_emType, pSvrMsgHead->m_To.m_nAgentID );
			break;
		}
		break;
	default:
		LOG("Svr=%d Socket=%d Recv Invalid MsgType=%d Error!", pSvrSock->GetSvrID(), pSvrSock->GetSocket(), pSvrMsgHead->m_nMsgType );
		return;
	}
}

void CServerParser::CheckSvrSocket()
{
	if (m_pUserSocket)
	{
		if (m_pUserSocket->IsConnectedSvr() == FALSE)
		{
			m_pUserSocket->ConnectToSvr();
			if (m_pUserSocket->IsConnectedSvr() == FALSE && (m_tLastSendGSMUser+SEC_MINUTE*5) <= GET_TIME_NOW)
			{
				m_tLastSendGSMUser = GET_TIME_NOW;
			}
		}
		else
			m_pUserSocket->SendKeepAlive();
	}
	if (m_pUserSocket2)
	{
		if (m_pUserSocket2->IsConnectedSvr() == FALSE)
		{
			m_pUserSocket2->ConnectToSvr();
			if (m_pUserSocket2->IsConnectedSvr() == FALSE && (m_tLastSendGSMUser + SEC_MINUTE * 5) <= GET_TIME_NOW)
			{
				m_tLastSendGSMUser = GET_TIME_NOW;
			}
		}
		else
			m_pUserSocket2->SendKeepAlive();
	}
}

void CServerParser::InitSvrSocket()
{
	InitUserSvrSocket();
	InitUserSvrSocket2();
}

void CServerParser::CloseServer()
{
	if (m_pUserSocket)
	{
		m_pUserSocket->ReclaimObj();
	}
	if (m_pUserSocket2)
	{
		m_pUserSocket2->ReclaimObj();
	}
}