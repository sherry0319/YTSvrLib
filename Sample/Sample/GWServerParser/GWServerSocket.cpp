#include "stdafx.h"
#include "GWServerSocket.h"
#include "GWServerParser.h"

CGWSvrSocket::CGWSvrSocket(void)
{
	m_recvBuf.SetBufSizeMax( 32 * 1024 * 1024 );
	m_recvBuf.ReSize( 8 * 1024 * 1024 );
}

CGWSvrSocket::~CGWSvrSocket(void)
{
}

void CGWSvrSocket::OnClosed()
{
	m_bClientClosed = TRUE;
	CGWSvrParser::GetInstance()->OnGWSvrDisconnect(this);
}

int CGWSvrSocket::OnRecved(const char* pBuf, int nLen)
{
	const int HEADER_SIZE = sizeof(sGWMsg_Head);
	const char* pHead = pBuf;
	int nPkgLen = 0;
	int nRead = 0;
	while (nLen >= HEADER_SIZE)
	{
		sGWMsg_Head* pMsgHead = (sGWMsg_Head*)pHead;
		nPkgLen = pMsgHead->m_nTotalMsgLen;

		if (pMsgHead->m_nTcpFlag != TCPFLAG_GWMSG || nPkgLen < HEADER_SIZE)
		{
			LOG("CGWSvrSocket=%x Msg=0x%x Data=%d/%d Head=%d error!", this, pMsgHead->m_nMsgType, nLen, nPkgLen, HEADER_SIZE);
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
	return nRead;
}

void CGWSvrSocket::PostMsg( const char* pBuf, int nLen )
{
	CGWSvrParser::GetInstance()->AddNewMessage(YTSvrLib::MSGTYPE_DATA, this, pBuf, nLen );
}
