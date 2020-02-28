#include "stdafx.h"
#include "GMSvrSocket.h"

CGMSvrSocket::CGMSvrSocket(void)
{
	m_recvBuf.ReSize(8 * 1024 * 1024);
}

CGMSvrSocket::~CGMSvrSocket(void){}

int CGMSvrSocket::OnRecved(const char* pBuf, int nLen)
{
	const int HEADER_SIZE = sizeof(SDBMsgHead);
	const char* pHead = pBuf;
	int nPkgLen = 0;
	int nRead = 0;
	while( nLen >= HEADER_SIZE )
	{   
		LPSDBMsgHead pMsgHead = (LPSDBMsgHead)pHead;
		UINT nDelData = 0;
		while( pMsgHead->m_dwTcpFlag != DBTCP_DEF_FLAG && nLen > HEADER_SIZE )
		{
			pHead++;
			nLen--;
			nDelData++;
			pMsgHead = (LPSDBMsgHead)pHead;
		}
		if( nDelData > 0 )
		{
			LOG("GMSocket=%d DelData=%d", GetSocket(), nDelData );
			nRead += nDelData;
		}
		nPkgLen = pMsgHead->m_dwLen;
		if( pMsgHead->m_dwTcpFlag != DBTCP_DEF_FLAG  )
			break;
		if ( nPkgLen > nLen )
		{
			break;
		}

		PostMsg(pHead, nPkgLen);
		pHead += nPkgLen;
		nLen = nLen - nPkgLen;
		nRead += nPkgLen;
	}
	return nRead;
}

void CGMSvrSocket::OnClosed()
{
	m_bClientClosed = TRUE;

	CGMSvrParser::GetInstance()->OnDisconnected(this);
}

void CGMSvrSocket::PostMsg( const char* pBuf, int nLen )
{
	CGMSvrParser::GetInstance()->AddNewMessage(YTSvrLib::MSGTYPE_DATA, this, pBuf, nLen );
}

void CGMSvrSocket::Send( const char* buf, int nLen )
{ 
 	LPSDBMsgHead pMsgHead = (LPSDBMsgHead)buf;

	YTSvrLib::ITCPBASE::Send(buf, nLen);
}