#include "stdafx.h"
#include "GWServerSocket.h"
#include "GWServerParser.h"

CGWSvrSocket::CGWSvrSocket(void)
{
	m_recvBuf.ReSize( 8 * 1024 * 1024 );
}

CGWSvrSocket::~CGWSvrSocket(void)
{
}

void CGWSvrSocket::OnClosed()
{
	if (!m_bClientClosed)
	{
		m_bClientClosed = TRUE;

		CGWSvrParser::GetInstance()->OnGWSvrDisconnect(this);
	}
}

int CGWSvrSocket::OnRecved(const char* pBuf, int nLen)
{
	const int HEADER_SIZE = sizeof(sGWMsg_Head);
	const char* pHead = pBuf;
	DWORD dwPkgLen = 0;
	int nRead = 0;
	while (nLen >= HEADER_SIZE)
	{
		sGWMsg_Head* pMsgHead = (sGWMsg_Head*)pHead;
		UINT nDelData = 0;
		while (pMsgHead->m_nTcpFlag != TCPFLAG_GWMSG && nLen > 0)
		{
			pHead++;
			nLen--;
			nDelData++;
			pMsgHead = (sGWMsg_Head*)pHead;
		}
		if (nDelData > 0)
		{
			LOG("GWSocket=%d DelData=%d", GetSocket(), nDelData);
			nRead += nDelData;
		}
		if (nLen < HEADER_SIZE) {
			break;
		}
		if (pMsgHead->m_nTcpFlag != TCPFLAG_GWMSG) {
			break;
		}
		dwPkgLen = pMsgHead->m_nTotalMsgLen;
		if (dwPkgLen > (DWORD)nLen)//内容不全
		{
			break;
		}
#ifdef _DEBUG_LOG
		LOG("GWSocket=%d Recv Msg=%d Len=%d", GetSocket(), pMsgHead->m_nMsgType, dwPkgLen);
#endif
		PostMsg(pHead, dwPkgLen);
		pHead += dwPkgLen;
		nLen = nLen - dwPkgLen;
		nRead += dwPkgLen;
	}
	return nRead;/*返回已消耗的数据长度*/
}

void CGWSvrSocket::PostMsg( const char* pBuf, int nLen )
{
	CGWSvrParser::GetInstance()->AddNewMessage( YTSvrLib::MSGTYPE_DATA, this, pBuf, nLen );
}