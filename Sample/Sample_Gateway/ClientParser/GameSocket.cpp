#include "stdafx.h"

void GameSocket::ReclaimObj()
{
	if (m_mapMessageRecved.size() > 0)
	{
		MessageRecord::iterator it = m_mapMessageRecved.begin();
		while (it != m_mapMessageRecved.end())
		{
			if (it->second)
			{
				LOG("Error : UNHANDLED Message Found : CID=%d MsgSeqno=%d MsgType=0x%04x",GetClientID(),it->first,it->second->m_nMsgType);
				CPkgParser::GetInstance()->ReleaseMessageTime(it->second);
			}
			++it;
		}

		m_mapMessageRecved.clear();
	}

	CPkgParser::GetInstance()->ReleaseConnector(this);
}
void GameSocket::OnDisconnect()
{
	if (!m_bClientClosed)
    {
		LOG("GameSocket=0x%08x Socket=%d DisConnected", this, GetSocket() );
		m_bClientClosed = TRUE;
		OnClientDisconnect( (GameSocket*)this );
		if (!m_bIsClosed)
		{
			SafeClose();
		}
    }    
}

void GameSocket::OnClosed()
{
	if (!m_bClientClosed)
	{
		LOG("GameSocket=0x%08x Socket=%d DisConnected", this, GetSocket());
		m_bClientClosed = TRUE;
		OnClientDisconnect((GameSocket*)this);
	}
}

int GameSocket::OnRecved( const char* pBuf, int nLen )
{
	const int HEADER_SIZE = sizeof(sClientMsg_Head);
	const char* pHead = pBuf;
	int nPkgLen = 0;
	int nRead = 0;
	while( nLen >= HEADER_SIZE )
	{   
		sClientMsg_Head* pMsgHead = (sClientMsg_Head*)pHead;
		nPkgLen = pMsgHead->m_nMsgLenTotal;
		if( nPkgLen > nLen )//ÄÚÈÝ²»È«
			break;
		if ( pMsgHead->m_nTCPFlag != TCPFLAG_SIGN_CLIENTMSG || nPkgLen < HEADER_SIZE )				
		{
			LOG("GameSock=%d parse Msg=0x%x Data=%d/%d error!", GetSocket(), pMsgHead->m_nMsgType, nLen, nPkgLen );
			break;
		}
		PostMsg(pHead, nPkgLen);
		pHead += nPkgLen;
		nLen = nLen - nPkgLen;

		nRead += nPkgLen;
	}

	if( nRead > 0 )
#ifdef LIB_WINDOWS
		m_tIdleExpired = _time32(NULL) + 600;
#else
		m_tIdleExpired = time(NULL) + 600;
#endif // LIB_WINDOWS
	return nRead;
}

void GameSocket::OnRecvNewMsg(UINT nMsgSeqno,UINT nMsgType)
{
	sMessageTime* pNewMessage = CPkgParser::GetInstance()->AllocateMessageTime();

	pNewMessage->m_nMsgType = nMsgType;
#ifdef LIB_WINDOWS
	GetSystemTimeAsFileTime(&(pNewMessage->m_RecvTime));

	UINT64 nBegin = *((UINT64*) (&pNewMessage->m_RecvTime));
#else
	gettimeofday(&pNewMessage->m_tvRecv,NULL);

	UINT64 nBegin = (UINT64) ((UINT64) ((UINT64) pNewMessage->m_tvRecv.tv_sec * 1000) + (UINT64) ((UINT64) pNewMessage->m_tvRecv.tv_usec / 1000));
#endif // LIB_WINDOWS

	LOG("OnRecvNewMsg : CID=%d Message: 0x%04x Begin : [%lld]",GetClientID(),nMsgType,nBegin);

	m_mapMessageRecved[nMsgSeqno] = pNewMessage;
}

void GameSocket::OnSendMsg(UINT nMsgSeqno,UINT nMsgType)
{
	nMsgType -= 0x1000;

	if (m_mapMessageRecved.find(nMsgSeqno) == m_mapMessageRecved.end())
	{
		LOG("CID=%d Error nMsgSeqno=%d Message : 0x%04x",GetClientID(),nMsgSeqno,nMsgType);
		return;
	}

	sMessageTime* pMessage = m_mapMessageRecved[nMsgSeqno];

	if (pMessage == NULL)
	{
		LOG("CID=%d Error : pMessage = NULL : MsgSeqno=%d MsgType=0x%04x",GetClientID(),nMsgSeqno,nMsgType);
		return;
	}

	if (pMessage->m_nMsgType == nMsgType)
	{
#ifdef LIB_WINDOWS
		FILETIME ftimeend;
		GetSystemTimeAsFileTime(&ftimeend);

		UINT64 nBegin = *((UINT64*)(&pMessage->m_RecvTime));
		UINT64 nEnd = *((UINT64*)(&ftimeend));

		UINT64 nTick = (UINT64)((nEnd - nBegin)/10000);
#else
		timeval tvEnd;
		gettimeofday(&tvEnd, NULL);

		UINT64 nBegin = (UINT64) ((UINT64) (pMessage->m_tvRecv.tv_sec * 1000) + (UINT64) (pMessage->m_tvRecv.tv_usec / 1000));
		UINT64 nEnd = (UINT64) ((UINT64) (tvEnd.tv_sec * 1000) + (UINT64) (tvEnd.tv_usec / 1000));

		UINT64 nTick = nEnd - nBegin;
#endif

#ifdef DEBUG64
		LOG("CID=%d Message Process Time : 0x%04x : CostTime : %lld",GetClientID(),nMsgType,nTick);
#endif
		if (nTick >= 500)
		{
			LOG("CID=%d Slow Message Found : 0x%04x . Begin[%lld] End[%lld] CostTime : %lld",GetClientID(),nMsgType,nBegin,nEnd,nTick);
		}
	}
	else
	{
		LOG("CID=%d Error : pMessage->m_nMsgType[0x%04x] != nMsgType[0x%04x]",GetClientID(),pMessage->m_nMsgType,nMsgType);
	}
	
	m_mapMessageRecved.erase(nMsgSeqno);
	CPkgParser::GetInstance()->ReleaseMessageTime(pMessage);
}

void GameSocket::PostMsg( const char* pBuf, int nLen )
{
    CPkgParser::GetInstance()->PostPkgMsg( this, pBuf, nLen );
}

void GameSocket::PostDisconnectMsg( EType eType )
{
    CPkgParser::GetInstance()->PostDisconnMsg( this, eType );
}

void GameSocket::Send( const char* buf, int nLen/*, BOOL bEncrypy, BOOL bIncSeqNo*/ )
{ 
	sClientMsg_Head* pMsgCommHead = (sClientMsg_Head*)buf;
	sClientMsg_RespHead* pRespHead = (sClientMsg_RespHead*)buf;

	LOGTRACE("Socket=%d Send Msg=0x%x Ret=%d Len=%d", GetSocket(), pRespHead->m_nMsgType, pRespHead->m_nRespRet, nLen);

	pMsgCommHead->m_nZipSrcLen = nLen-sizeof(sClientMsg_Head);
	if( CConfig::GetInstance()->m_nMinClientMsgToZip != 0 
		&& nLen >= CConfig::GetInstance()->m_nMinClientMsgToZip 
		&& nLen < MAX_DATA_TO_ZIP )
	{
		BYTE szZipped[MAX_DATA_TO_ZIP+1024];
		memcpy( szZipped, pMsgCommHead, sizeof(sClientMsg_Head) );
		BYTE* pSrcData = (BYTE*)(pMsgCommHead+1);
		pMsgCommHead = (sClientMsg_Head*)szZipped;
		BYTE* pDstData = (BYTE*)(pMsgCommHead+1);

		ULONG nZippedLen = sizeof(szZipped) - sizeof(sClientMsg_Head);
		int nRet = compress(	pDstData, 
																&nZippedLen, 
																pSrcData, 
																pMsgCommHead->m_nZipSrcLen );
		if( nRet == Z_OK )
		{
			pMsgCommHead->m_nZipEncrypFlag |= _CLIENTMSG_FLAG_ZIPPED;
			pMsgCommHead->m_nMsgLenTotal = sizeof(sClientMsg_Head) + nZippedLen;
			LOG("Socket=%d Send Msg=0x%04x SrcLen=%d Zipped=%d", GetSocket(), pMsgCommHead->m_nMsgType, nLen, pMsgCommHead->m_nMsgLenTotal );

			YTSvrLib::ITCPBASE::Send( (LPCSTR)szZipped, pMsgCommHead->m_nMsgLenTotal );
			return;
		}
	}

	YTSvrLib::ITCPBASE::Send(buf, nLen);
}

void GameSocket::Send( const std::string& strPkg )
{ 
	Send( strPkg.c_str(), (int)strPkg.size() );
}

void GameSocket::Send( const std::string* pStrPkg )
{ 
	Send( pStrPkg->c_str(), (int)pStrPkg->size() );
}

void GameSocket::Init()
{
	YTSvrLib::ITCPCONNECTOR::Clean();
	m_tIdleExpired = 0;
	m_nClientID = 0;

	m_mapMessageRecved.clear();
	m_bClientClosed = FALSE;
}

int GameSocket::OnSocketRecv()
{
	char* buf = NULL;
	int nMaxlen = 0;
	if (m_recvBuf.GetLength() <= 0)
	{
		buf = m_recvBuf.GetBuffer();
		nMaxlen = sizeof(sClientMsg_Head);
	}
	else
	{
		size_t nRecved = m_recvBuf.GetLength();
		size_t nErrorData = 0;
		while (nRecved >= sizeof(sClientMsg_Head))
		{
			sClientMsg_Head* pMsgHead = (sClientMsg_Head*) (m_recvBuf.GetBuffer() + nErrorData);
			if (pMsgHead->m_nTCPFlag == TCPFLAG_SIGN_CLIENTMSG)
				break;
			nErrorData++;
			nRecved--;
		}
		if (nErrorData > 0)
		{
			LOG("GameSocket=%d(0x%08x) Recv() ErrorData=%d!", GetSocket(), this, nErrorData);
			m_recvBuf.ReleaseBuffer(nErrorData);
		}
		buf = m_recvBuf.GetBuffer() + m_recvBuf.GetLength();
		if (m_recvBuf.GetLength() >= sizeof(sClientMsg_Head))
		{
			sClientMsg_Head* pMsgHead = (sClientMsg_Head*) m_recvBuf.GetBuffer();
			if (pMsgHead->m_nMsgLenTotal > (UINT) m_recvBuf.GetLength())
				nMaxlen = pMsgHead->m_nMsgLenTotal - (UINT) m_recvBuf.GetLength();
			else
				nMaxlen = sizeof(sClientMsg_Head);
		}
		else
		{
			nMaxlen = (ULONG) (sizeof(sClientMsg_Head) - m_recvBuf.GetLength());
		}
	}
	if (nMaxlen > m_recvBuf.GetIdleLength())
	{
		LOG("GameSocket=%d(0x%08x) Recv() Need Data=%d > Buff=%d Error!", GetSocket(), this, nMaxlen, m_recvBuf.GetIdleLength());
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