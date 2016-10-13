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
	
}

void CGWSvrSocket::ReclaimObj()
{
	CGWSvrParser::GetInstance()->ReleaseConnector(this);
}

int CGWSvrSocket::OnRecved(const char* pBuf, int nLen)
{
	const int HEADER_SIZE = sizeof(sGWMsg_Head);
	const char* pHead = pBuf;
	DWORD dwPkgLen = 0;
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
			LOG("GWSocket=%d DelData=%d", GetSocket(), nDelData );
			nRead += nDelData;
		}
		if( pMsgHead->m_nTcpFlag != TCPFLAG_GWMSG )
			break;
		dwPkgLen = pMsgHead->m_nTotalMsgLen;
		if ( dwPkgLen > (DWORD)nLen )//ÄÚÈÝ²»È«
		{
			break;
		}
		++m_nRecvSeqNo;
		PostMsg(pHead, dwPkgLen);
		pHead += dwPkgLen;
		nLen = nLen - dwPkgLen;
		nRead += dwPkgLen;
	}
	return nRead;
}

void CGWSvrSocket::PostMsg( const char* pBuf, int nLen )
{
	CGWSvrParser::GetInstance()->PostPkgMsg( this, pBuf, nLen );
}

void CGWSvrSocket::PostDisconnectMsg( EType eType )
{
	CGWSvrParser::GetInstance()->PostDisconnMsg( this, eType );
}

int CGWSvrSocket::OnSocketRecv()
{
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
		LOG("GWSocket=%d(0x%08x) Recv() Need Data=%d > Buff=%d Error!", GetSocket(), this, nMaxlen , m_recvBuf.GetIdleLength());
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

	return (int)nRealRead;
}