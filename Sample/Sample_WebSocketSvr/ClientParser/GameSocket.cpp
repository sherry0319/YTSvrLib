#include "stdafx.h"

void GameSocket::OnClosed()
{
	LOG("GameSocket::OnClosed = 0x%x", this);
	if (m_mapMessageRecved.size() > 0)
	{
		MessageRecord::iterator it = m_mapMessageRecved.begin();
		while (it != m_mapMessageRecved.end())
		{
			if (it->second)
			{
				LOG("Error : UNHANDLED Message Found : CID=%d MsgSeqno=%d MsgType=%d", GetClientID(), it->first, it->second->m_nMsgType);
				CPkgParser::GetInstance()->ReleaseMessageTime(it->second);
			}
			++it;
		}

		m_mapMessageRecved.clear();
	}
}

void GameSocket::Create(YTSvrLib::IWSSERVER* server, lws* ctx, YTSvrLib::IWSSESSION* session)
{
	IWSCONNECTOR::Create(server, ctx, session);

	strncpy_s(m_szIP, GetIP(), 31);
	m_nPort = GetPort();
	m_Sock = GetSocket();
}

void GameSocket::OnRecvNewMsg(UINT nMsgSeqno, int nMsgType)
{
	sMessageTime* pNewMessage = CPkgParser::GetInstance()->AllocateMessageTime();

	pNewMessage->m_nMsgType = nMsgType;
#ifdef LIB_WINDOWS
	GetSystemTimeAsFileTime(&(pNewMessage->m_RecvTime));

	UINT64 nBegin = *((UINT64*) (&pNewMessage->m_RecvTime));
#else
	gettimeofday(&pNewMessage->m_tvRecv, NULL);

	UINT64 nBegin = (UINT64) ((UINT64) ((UINT64) pNewMessage->m_tvRecv.tv_sec * 1000) + (UINT64) ((UINT64) pNewMessage->m_tvRecv.tv_usec / 1000));
#endif // LIB_WINDOWS

	LOG("OnRecvNewMsg : CID=%d Message: %d Begin : [%lld]", GetClientID(), nMsgType, nBegin);

	m_mapMessageRecved[nMsgSeqno] = pNewMessage;
}

void GameSocket::OnSendMsg(UINT nMsgSeqno, int nMsgType)
{
	nMsgType -= RESP(0);

	if (m_mapMessageRecved.find(nMsgSeqno) == m_mapMessageRecved.end())
	{
		LOG("CID=%d Error nMsgSeqno=%d Message : %d", GetClientID(), nMsgSeqno, nMsgType);
		return;
	}

	sMessageTime* pMessage = m_mapMessageRecved[nMsgSeqno];

	if (pMessage == NULL)
	{
		LOG("CID=%d Error : pMessage = NULL : MsgSeqno=%d MsgType=%d", GetClientID(), nMsgSeqno, nMsgType);
		return;
	}

	if (pMessage->m_nMsgType == nMsgType)
	{
#ifdef LIB_WINDOWS
		FILETIME ftimeend;
		GetSystemTimeAsFileTime(&ftimeend);

		UINT64 nBegin = *((UINT64*) (&pMessage->m_RecvTime));
		UINT64 nEnd = *((UINT64*) (&ftimeend));

		UINT64 nTick = (UINT64) ((nEnd - nBegin) / 10000);
#else
		timeval tvEnd;
		gettimeofday(&tvEnd, NULL);

		UINT64 nBegin = (UINT64) ((UINT64) (pMessage->m_tvRecv.tv_sec * 1000) + (UINT64) (pMessage->m_tvRecv.tv_usec / 1000));
		UINT64 nEnd = (UINT64) ((UINT64) (tvEnd.tv_sec * 1000) + (UINT64) (tvEnd.tv_usec / 1000));

		UINT64 nTick = nEnd - nBegin;
#endif

#ifdef DEBUG64
		LOG("CID=%d Message Process Time : %d : CostTime : %lld", GetClientID(), nMsgType, nTick);
#endif
		if (nTick >= 500)
		{
			LOG("CID=%d Slow Message Found : %d . Begin[%lld] End[%lld] CostTime : %lld", GetClientID(), nMsgType, nBegin, nEnd, nTick);
		}
	}
	else
	{
		LOG("CID=%d Error : pMessage->m_nMsgType[%d] != nMsgType[%d]", GetClientID(), pMessage->m_nMsgType, nMsgType);
	}

	m_mapMessageRecved.erase(nMsgSeqno);
	CPkgParser::GetInstance()->ReleaseMessageTime(pMessage);
}

void GameSocket::Init()
{
	YTSvrLib::IWSCONNECTOR::Clean();

	m_nClientID = 0;
	m_tExpired = 0;
	m_mapMessageRecved.clear();
	m_bClientClosed = FALSE;
	m_Sock = 0;
	m_nPort = 0;
	ZeroMemory(m_szIP, sizeof(m_szIP));
}

void GameSocket::SendText(const char* info, int len)
{
	YTSvrLib::IWSCONNECTOR::Send(info, len, LWS_WRITE_TEXT);
}

void GameSocket::SendBinary(const char* info, int len)
{
	sClientMsg_RespHead* pRespHead = (sClientMsg_RespHead*) info;
	if (pRespHead)
	{
		LOGTRACE("Socket=%x Send Msg=%d Ret=%d Len=%d", this, pRespHead->m_nMsgType, pRespHead->m_nRespRet, len);
	}

	YTSvrLib::IWSCONNECTOR::Send(info, len, LWS_WRITE_BINARY);
}