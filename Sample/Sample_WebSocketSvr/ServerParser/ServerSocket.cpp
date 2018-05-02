#include "stdafx.h"
#include "ServerSocket.h"

CServerSocket::CServerSocket(void)
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

void CServerSocket::ReclaimObj()
{
	CServerParser::GetInstance()->ReclaimObj(this);
}

int CServerSocket::OnSocketRecv()
{// TCPFLAG_GWMSG sGWMsg_Head
	char* buf = NULL;
	int nMaxlen = 0;
	if (m_recvBuf.GetLength() <= 0)
	{
		buf = m_recvBuf.GetBuffer();
		nMaxlen = sizeof(sGWMsg_Head);
	}
	else
	{
		size_t nRecved = m_recvBuf.GetLength();
		size_t nErrorData = 0;
		while (nRecved >= sizeof(sGWMsg_Head))
		{
			sGWMsg_Head* pMsgHead = (sGWMsg_Head*) (m_recvBuf.GetBuffer() + nErrorData);
			if (pMsgHead->m_nTcpFlag == TCPFLAG_GWMSG)
				break;
			nErrorData++;
			nRecved--;
		}
		if (nErrorData > 0)
		{
			LOG("GWSocket=%d(0x%08x) Recv() ErrorData=%d!", GetSocket(), this, nErrorData);
			m_recvBuf.ReleaseBuffer(nErrorData);
		}
		buf = m_recvBuf.GetBuffer() + m_recvBuf.GetLength();
		if (m_recvBuf.GetLength() >= sizeof(sGWMsg_Head))
		{
			sGWMsg_Head* pMsgHead = (sGWMsg_Head*) m_recvBuf.GetBuffer();
			if (pMsgHead->m_nTotalMsgLen > (UINT) m_recvBuf.GetLength())
				nMaxlen = pMsgHead->m_nTotalMsgLen - (UINT) m_recvBuf.GetLength();
			else
				nMaxlen = sizeof(sGWMsg_Head);
		}
		else
		{
			nMaxlen = (ULONG) (sizeof(sGWMsg_Head) - m_recvBuf.GetLength());
		}
	}
	if (nMaxlen > m_recvBuf.GetIdleLength())
	{
		LOG("GWSocket=%d(0x%08x) Recv() Need Data=%d > Buff=%d Error!", GetSocket(), this, nMaxlen, m_recvBuf.GetIdleLength());
		SafeClose();
		return -1;
	}

	size_t nRealRead = 0;
	
	if (nMaxlen > 0)
	{
		nRealRead = bufferevent_read(m_pbufferevent, buf, nMaxlen);

		if (nRealRead == 0)
		{
			SafeClose();
			return -1;
		}

		m_recvBuf.AddBuffer(nRealRead);
	}

	return (int) nRealRead;
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
		UINT nDelData = 0;
		while( pMsgHead->m_nTcpFlag != TCPFLAG_GWMSG && nLen > HEADER_SIZE )
		{
			pHead++;
			nLen--;
			nDelData++;
			pMsgHead = (sGWMsg_Head*)pHead;
		}
		if( nDelData > 0 )
		{
			LOG("Svr=%d Socket=%d DelData=%d", GetSvrID(), GetSocket(), nDelData );
			nRead += nDelData;
		}
		if( pMsgHead->m_nTcpFlag != TCPFLAG_GWMSG )
			break;
		nPkgLen = pMsgHead->m_nTotalMsgLen;
		if( nPkgLen > nLen || nPkgLen <= 0 )//内容不全
		{
			break;
		}

		PostMsg(pHead, nPkgLen);
		pHead += nPkgLen;
		nLen = nLen - nPkgLen;
		nRead += nPkgLen;
	}
	return nRead;/*(pHead - pBuf);*/
}

void CServerSocket::OnDisconnect()
{
	m_bIsClosed = TRUE;
	m_bIsConnecting = FALSE;
	m_bConnectedSvr = FALSE;
	m_sendBuf.Clear();
	m_recvBuf.Clear();
	LOG( "Svr=%d Socket=%d OnDisconnected", GetSvrID(), GetSocket() );
	//TODO
	CServerParser::GetInstance()->OnSvrDisconnect( this );
	SafeClose();
}

void CServerSocket::OnClosed()
{

}

void CServerSocket::Init()
{
	YTSvrLib::ITCPCLIENT::Clean();
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
	CServerParser::GetInstance()->PostPkgMsg( this, pBuf, nLen );
}
void CServerSocket::PostDisconnectMsg( EType eType )
{
	CServerParser::GetInstance()->PostDisconnMsg( this, eType );
}

void CServerSocket::SetSvrInfo( EM_AGENT emSvrType, int nSvrID, LPCSTR pszSvrIPAddr, int nSvrTcpPort )
{
	m_emSvrType = emSvrType;
	m_nSvrID = nSvrID;
	strncpy_s( m_szSvrIPAddr, pszSvrIPAddr, 15 );
	m_szSvrIPAddr[15] = '\0';
	m_nSvrTcpPort = nSvrTcpPort;
	LOG("Svr=%d Type=%d IP=%s:%d SetSvrInfo.", GetSvrID(), GetSvrType(), m_szSvrIPAddr, m_nSvrTcpPort );
}

BOOL CServerSocket::ConnectToSvr()
{
	if( IsConnectedSvr() )
	{
		LOG("Svr=%d Socket=%d IsConnected already.", GetSvrID(), GetSocket() );
		return FALSE;
	}
	if( m_bIsConnecting )
	{
		m_nConnectingTimes++;
		LOG("Svr=%d Socket=%d Is Connecting.", GetSvrID(), GetSocket() );

		if (m_nConnectingTimes >= MAX_CONNECTING_RETRY)
		{
			m_bIsClosed = TRUE;
			m_bIsConnecting = FALSE;
			m_bConnectedSvr = FALSE;
			m_sendBuf.Clear();
			m_recvBuf.Clear();
			m_nConnectingTimes = 0;
			SafeClose();
			return FALSE;
		}

		return TRUE;
	}

	if( false == CreateClient(	m_szSvrIPAddr, m_nSvrTcpPort ) )
	{
		LOG("Svr=%d Socket=%d Connect Svr=%s:%d Error!", GetSvrID(), GetSocket(), m_szSvrIPAddr, m_nSvrTcpPort );
		return FALSE;
	}

	m_nConnectingTimes = 0;
	LOG("Svr=%d Socket=%d Connecting Svr=%s:%d...", GetSvrID(), GetSocket(), m_szSvrIPAddr, m_nSvrTcpPort );
	return TRUE;
}

void CServerSocket::OnConnected()
{//连接成功的回调
	YTSvrLib::ITCPCLIENT::OnConnected();

	m_bConnectedSvr = TRUE;
	m_sendBuf.Clear();
	m_recvBuf.Clear();

	LOG("Svr=%d Socket=%d Connected Svr=%s:%d", GetSvrID(), GetSocket(), m_szSvrIPAddr, m_nSvrTcpPort);

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