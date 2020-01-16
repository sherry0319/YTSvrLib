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

void GameSocket::OnClosed()
{
	if (!m_bClientClosed)
	{
		LOG("GameSocket=%x Closed", this);
		m_bClientClosed = TRUE;
		OnClientDisconnect((GameSocket*)this);
		ReclaimObj();
	}
}

int GameSocket::OnRecved( const char* pBuf, int nLen )
{
	int nRead = 0;
	const int HEADER_SIZE = sizeof(sClientMsg_Head);
	const char* pHead = pBuf;
	int nPkgLen = 0;
	while (nLen >= HEADER_SIZE)
	{
		sClientMsg_Head* pMsgHead = (sClientMsg_Head*)pHead;
		int nDelData = 0;
		while (pMsgHead->m_nTCPFlag != TCPFLAG_SIGN_CLIENTMSG && nLen > 0)
		{
			pHead++;
			nLen--;
			nDelData++;
			pMsgHead = (sClientMsg_Head*)pHead;
		}
		if (nDelData > 0)
		{
			LOG("GameSocket=0x%x[%d] DelData=%d", this, GetSocket(), nDelData);
			nRead += nDelData;
		}
		if (nLen < HEADER_SIZE)
		{
			break;
		}
		nPkgLen = pMsgHead->m_nMsgLenTotal;

		if (nPkgLen > nLen)//ÄÚÈÝ²»È«
			break;

		PostMsg(pHead, nPkgLen);
		pHead += nPkgLen;
		nLen -= nPkgLen;
		nRead += nPkgLen;
	}

	if (nRead > 0)
	{
		m_tIdleExpired = time32() + DEFAULT_KEEP_ALIVE_EXPIRED;
	}
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
	CPkgParser::GetInstance()->AddNewMessage(YTSvrLib::MSGTYPE_DATA, this, pBuf, nLen);
}

void GameSocket::Send( const char* buf, int nLen/*, BOOL bEncrypy, BOOL bIncSeqNo*/ )
{ 
	sClientMsg_Head* pMsgCommHead = (sClientMsg_Head*)buf;
	sClientMsg_RespHead* pRespHead = (sClientMsg_RespHead*)buf;

	LOGTRACE("Send Msg=%d Ret=%d Len=%d", pRespHead->m_nMsgType, pRespHead->m_nRespRet, nLen);

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
			LOG("Socket=%x Send Msg=%x SrcLen=%d Zipped=%d", this, pMsgCommHead->m_nMsgType, nLen, pMsgCommHead->m_nMsgLenTotal );

			YTSvrLib::ITCPBASE::Send( (LPCSTR)szZipped, pMsgCommHead->m_nMsgLenTotal );
			return;
		}
	}

	YTSvrLib::ITCPBASE::Send(buf, nLen);
}

void GameSocket::Init()
{
	YTSvrLib::ITCPBASE::Clean();
	m_tIdleExpired = 0;
	m_nClientID = 0;

	m_mapMessageRecved.clear();
	m_bClientClosed = FALSE;
}
