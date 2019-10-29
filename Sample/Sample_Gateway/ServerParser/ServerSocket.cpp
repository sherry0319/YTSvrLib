#include "stdafx.h"
#include "ServerSocket.h"

CServerSocket::CServerSocket(void) : ITCPCLIENT()
{
	m_sendBuf.SetQueueLenMax( 8192 );
	m_recvBuf.ReSize( 1024 * 128 );
	m_nUserCnt = 0;
	m_nConnectingTimes = 0;
	m_bConnectedSvr = FALSE;
}

CServerSocket::~CServerSocket(void)
{
}

int CServerSocket::OnRecved(const char* pBuf, int nLen)
{
	const int HEADER_SIZE = sizeof(sGWMsg_Head);
	const char* pHead = pBuf;
	int nPkgLen = 0;
	int nRead = 0;
	while( nLen >= HEADER_SIZE )
	{   
		sGWMsg_Head* pMsgHead = (sGWMsg_Head*)pHead;
		nPkgLen = pMsgHead->m_nTotalMsgLen;

		if ( pMsgHead->m_nTcpFlag != TCPFLAG_GWMSG || nPkgLen < HEADER_SIZE)
		{
			LOG("GameSock=%x Msg=0x%x Data=%d/%d Head=%d error!", this, pMsgHead->m_nMsgType, nLen, nPkgLen, HEADER_SIZE);
			int nOffset = 0;
			while (nLen > 0)
			{
				nLen--;
				nOffset++;
				pMsgHead = (sGWMsg_Head*)(pHead + nOffset);
				if (pMsgHead->m_nTcpFlag == TCPFLAG_GWMSG && pMsgHead->m_nTotalMsgLen >= HEADER_SIZE)
				{
					break;
				}
				nRead += nOffset;
				pHead += nOffset;
			}
			if (nLen == 0)
			{
				break;
			}
		}

		PostMsg(pHead, nPkgLen);
		pHead += nPkgLen;
		nLen -= nPkgLen;
		nRead += nPkgLen;
	}
	return nRead;/*(pHead - pBuf);*/
}

void CServerSocket::OnClosed()
{
	m_bConnectedSvr = FALSE;
	m_sendBuf.Clear();
	m_recvBuf.Clear();
	LOG("Svr=%d Socket=0x%x OnDisconnected", GetSvrID(), this);
	//TODO
	CServerParser::GetInstance()->OnSvrDisconnect(this);
}

void CServerSocket::Init()
{
	m_bConnectedSvr = FALSE;
	m_emSvrType = emAgent_Null;
	m_nSvrID = 0;
	m_szSvrIPAddr[0] = '\0';
	m_nSvrTcpPort = 0;
	m_nUserCnt = 0;
	m_nConnectingTimes = 0;
	//ZeroMemory( &m_unParam, sizeof(m_unParam) );
}

void CServerSocket::PostMsg( const char* pBuf, int nLen )
{
	CServerParser::GetInstance()->AddNewMessage( YTSvrLib::MSGTYPE_DATA,this, pBuf, nLen );
}

void CServerSocket::SetSvrInfo( EM_AGENT emSvrType, int nSvrID, LPCSTR destHost, int nPort)
{
	m_emSvrType = emSvrType;
	m_nSvrID = nSvrID;
	strncpy_s(m_szSvrIPAddr, destHost, 15);
	m_nSvrTcpPort = nPort;
	LOG("Svr=%d Type=%d Host=[%s:%d] SetSvrInfo.", GetSvrID(), GetSvrType(), destHost, nPort);
}

BOOL CServerSocket::ConnectToSvr() {
	if (IsConnectedSvr())
	{
		LOG("Svr=%d Socket=0x%x IsConnected already.", GetSvrID(), this);
		return FALSE;
	}
	if (IsConnecting())
	{
		m_nConnectingTimes++;
		LOG("Svr=%d Socket=0x%x Is Connecting.", GetSvrID(), this);

		if (m_nConnectingTimes >= MAX_CONNECTING_RETRY)
		{
			m_bConnectedSvr = FALSE;
			m_sendBuf.Clear();
			m_recvBuf.Clear();
			m_nConnectingTimes = 0;
			SafeClose();
			return FALSE;
		}

		return TRUE;
	}

	Connect(CServerParser::GetInstance(),m_szSvrIPAddr, m_nSvrTcpPort);

	m_nConnectingTimes = 0;
	LOG("Svr=%d Socket=0x%x Connecting Svr=%s:%d...", GetSvrID(), this, m_szSvrIPAddr, m_nSvrTcpPort);
	return TRUE;
}

void CServerSocket::OnConnectFailed() {
	m_bConnectedSvr = FALSE;
	m_nConnectingTimes = 0;
	m_sendBuf.Clear();
	m_recvBuf.Clear();
	LOG("Svr=%d Socket=0x%x Connect Failed!! Svr=%s:%d", GetSvrID(), this, m_szSvrIPAddr, m_nSvrTcpPort);
}

void CServerSocket::OnConnected()
{//连接成功的回调
	m_bConnectedSvr = TRUE;
	m_sendBuf.Clear();
	m_recvBuf.Clear();

	LOG("Svr=%d Socket=0x%x Connected Svr=%s:%d", GetSvrID(), this, m_szSvrIPAddr, m_nSvrTcpPort);

	sGWMsg_Login sMsgLogin;
	sMsgLogin.m_From.m_emType = emAgent_GateWay;
	sMsgLogin.m_From.m_nAgentID = CConfig::GetInstance()->m_nLocalSvrID;
	sMsgLogin.m_To.m_emType = m_emSvrType;
	sMsgLogin.m_To.m_nAgentID = GetSvrID();
	sMsgLogin.m_nGameID = CConfig::GetInstance()->m_nGameID;
	sMsgLogin.m_nPublicSvrID = CConfig::GetInstance()->m_nPublicSvrID;

	Send((const char*)&sMsgLogin,sizeof(sMsgLogin));
}

void CServerSocket::SendKeepAlive()
{
	sGWMsg_KeepAlive sMsg;
	sMsg.m_From.m_emType = emAgent_GateWay;
	sMsg.m_From.m_nAgentID = CConfig::GetInstance()->m_nLocalSvrID;
	sMsg.m_To.m_emType = emAgent_UserSvr;
	sMsg.m_To.m_nAgentID = GetSvrID();
	Send( (char*)&sMsg, sizeof(sMsg) );
}

void CServerSocket::SendMsgToSvr(EM_AGENT emFromAgtType, const char* pszClientIP, UINT nFromAgtID, LPCSTR pszMsg, int nMsgLen)
{
	char szBuf[1024] = {0};
	sGWMsg_Head* pMsgHead = (sGWMsg_Head*)szBuf;
	if( emFromAgtType == emAgent_Client )
		pMsgHead->Init( T_GWMSG_C2S, sizeof(sGWMsg_Head) + nMsgLen );
	else
		pMsgHead->Init( T_GWMSG_S2S, sizeof(sGWMsg_Head) + nMsgLen );

	strncpy_s(pMsgHead->m_szClientIP, pszClientIP, 31);
	pMsgHead->m_From.m_emType = emFromAgtType;
	pMsgHead->m_From.m_nAgentID = nFromAgtID;
	pMsgHead->m_RouteBy.m_emType = emAgent_GateWay;
	pMsgHead->m_RouteBy.m_nAgentID = CConfig::GetInstance()->m_nLocalSvrID;
	pMsgHead->m_To.m_emType = GetSvrType();
	pMsgHead->m_To.m_nAgentID = GetSvrID();
	Send( szBuf, sizeof(sGWMsg_Head) );
	Send( pszMsg, nMsgLen );
}