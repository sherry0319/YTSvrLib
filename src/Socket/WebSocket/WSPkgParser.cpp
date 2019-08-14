#include "stdafx.h"
#include "WSPkgParser.h"

namespace YTSvrLib {
	CWSParserBase::CWSParserBase() : m_PoolMsgPkg("WSMSG"), m_PoolDisconnectPkg("WSEVENT")
	{

	}

	void CWSParserBase::onWSMsgRecv()
	{
		WSMSG* pMsgPkg = NULL;
		int nCount = 0;
		BOOL bNeedSetEvt = FALSE;

		for (;;)
		{
			m_qMsg.Lock();
			if (m_qMsg.empty())
			{
				m_qMsg.UnLock();
				break;
			}
			if (nCount >= 128)
			{	//避免其他事件队列堵塞
				bNeedSetEvt = TRUE;
				m_qMsg.UnLock();
				break;
			}
			pMsgPkg = m_qMsg.pop_front();
			m_qMsg.UnLock();

			ProcessMessage(pMsgPkg->pConn, pMsgPkg->msg.c_str(), (int)pMsgPkg->msg.size());

			m_PoolMsgPkg.ReclaimObj(pMsgPkg);
			nCount++;
		}
		if (bNeedSetEvt)
			SetEvent();
	}

	void CWSParserBase::onWSEventRecv()
	{
		WSEVENT* pPkg = NULL;

		for (;;)
		{
			m_qDisconnectMsg.Lock();
			if (m_qDisconnectMsg.empty())
			{
				m_qDisconnectMsg.UnLock();
				break;
			}
			pPkg = m_qDisconnectMsg.pop_front();
			m_qDisconnectMsg.UnLock();
			switch (pPkg->eType)
			{
			case WSEType_ClientAccept: {
				ProcessAcceptedMsg(pPkg->pConn);
			}break;
			case WSEType_ClientClose: {
				ProcessDisconnectMsg(pPkg->pConn);
			}break;
			default:break;
			}

			m_PoolDisconnectPkg.ReclaimObj(pPkg);
		}
	}

	void CWSParserBase::postWSMsg(IWSCONNECTOR* pConn, const char* msg, int len)
	{
		WSMSG* pMsgPkg = m_PoolMsgPkg.ApplyObj();
		if (pMsgPkg)
		{
			pMsgPkg->msg.assign(msg, len);
			pMsgPkg->pConn = pConn;
			addWSMsg(pMsgPkg);
		}
	}

	void CWSParserBase::postWSEvent(IWSCONNECTOR* pConn, WSEType type)
	{
		WSEVENT* pPkg = m_PoolDisconnectPkg.ApplyObj();
		if (pPkg)
		{
			pPkg->pConn = pConn;
			pPkg->eType = type;
			addWSEvent(pPkg);
		}
	}

	void CWSParserBase::addWSMsg(WSMSG* pPkg)
	{
		BOOL bMustSetEvent = FALSE;
		m_qMsg.Lock();
		if (m_qMsg.empty())
		{
			bMustSetEvent = TRUE;
		}
		m_qMsg.push_back(pPkg);
		m_qMsg.UnLock();
		if (bMustSetEvent)
		{
			SetEvent();
		}
	}

	void CWSParserBase::addWSEvent(WSEVENT* pPkg)
	{
		BOOL bMustSetEvent = FALSE;
		m_qDisconnectMsg.Lock();
		if (m_qDisconnectMsg.empty())
		{
			bMustSetEvent = TRUE;
		}
		m_qDisconnectMsg.push_back(pPkg);
		m_qDisconnectMsg.UnLock();
		if (bMustSetEvent)
		{
			SetDisconnectEvent();
		}
	}
}