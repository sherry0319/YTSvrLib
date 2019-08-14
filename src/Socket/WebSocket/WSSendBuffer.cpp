#include "stdafx.h"
#include "WSSendBuffer.h"

namespace YTSvrLib {
	CWSSendBuffer::CWSSendBuffer() : m_poolBlock("WSSndBuffBlock")
	{
		m_pBlockSending = NULL;
		m_nQueueLenMax = 0;
	}

	CWSSendBuffer::~CWSSendBuffer()
	{

	}

	WSSndBuffBlock* CWSSendBuffer::AllocateBlock()
	{
		return m_poolBlock.ApplyObj();
	}
	void CWSSendBuffer::ReleaseBlock(WSSndBuffBlock* pObj)
	{
		m_poolBlock.ReclaimObj(pObj);
	}

	BOOL CWSSendBuffer::IsSending()
	{
		return (m_pBlockSending != NULL);
	}

	BOOL CWSSendBuffer::AddBlock(const char* buf, int len)
	{
		if (m_pBlockSending == NULL)
		{
			m_pBlockSending = AllocateBlock();
			if (m_pBlockSending == NULL)
			{
				LOG("Allocate sSndBufferBlock Failed : %d", GetLastError());
				return FALSE;
			}
			m_pBlockSending->SetMsg(buf, len);
		}
		else if (IsQueueFulled())
		{
			LOG("The Queue Is Fulled CurQueue=%d MaxQueue=%d", GetQueueLen(), m_nQueueLenMax);
			return FALSE;
		}
		else
		{
			WSSndBuffBlock* pBlock = AllocateBlock();
			if (pBlock == NULL)
			{
				LOG("Allocate sSndBufferBlock Failed : %d", GetLastError());
				return FALSE;
			}
			pBlock->SetMsg(buf, len);
			m_queueSnd.push_back(pBlock);
		}
		return TRUE;
	}
	const char* CWSSendBuffer::GetDataToSend()
	{
		if (m_pBlockSending)
			return m_pBlockSending->GetMsgData();
		return NULL;
	}
	int CWSSendBuffer::GetDataLenToSend()
	{
		if (m_pBlockSending)
			return m_pBlockSending->GetMsgLen();
		return 0;
	}

	BOOL CWSSendBuffer::OnSend()
	{	//仍有数据要发送，返回TRUE，否则返回FALSE
		if (m_pBlockSending)
		{
			ReleaseBlock(m_pBlockSending);
			m_pBlockSending = NULL;
		}
		while (m_pBlockSending == NULL && m_queueSnd.size() > 0)
		{
			m_pBlockSending = m_queueSnd.pop_front();
			if (m_pBlockSending->GetMsgLen() <= 0)
			{
				ReleaseBlock(m_pBlockSending);
				m_pBlockSending = NULL;
			}
		}
		if (m_pBlockSending)
			return TRUE;
		return FALSE;
	}
	void CWSSendBuffer::Clear()
	{
		if (m_pBlockSending)
		{
			ReleaseBlock(m_pBlockSending);
			m_pBlockSending = NULL;
		}
		while (m_queueSnd.size() > 0)
		{
			WSSndBuffBlock* pBlock = m_queueSnd.pop_front();
			if (pBlock)
			{
				ReleaseBlock(pBlock);
				pBlock = NULL;
			}
		}
	}
	int CWSSendBuffer::GetQueueLen()
	{
		return m_queueSnd.size();
	}
	void CWSSendBuffer::SetQueueLenMax(int nMax)
	{
		m_nQueueLenMax = nMax;
	}
	BOOL CWSSendBuffer::IsQueueFulled()
	{
		if (m_nQueueLenMax <= 0)
			return FALSE;
		return (GetQueueLen() > m_nQueueLenMax);
	}
}