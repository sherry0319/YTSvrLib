/*MIT License

Copyright (c) 2016 Archer Xu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#include "stdafx.h"
#include "SendBuffer.h"

namespace YTSvrLib
{
	void sSndBufferBlock::Init()
	{
		m_nDataLen = 0;
	}
	char* sSndBufferBlock::GetBlock()
	{
		return m_szBlock;
	}
	int sSndBufferBlock::GetDataLen()
	{
		return m_nDataLen;
	}
	int sSndBufferBlock::SetData(LPCSTR pszData, int nLen)
	{
		if (pszData == NULL || nLen <= 0)
			return 0;
		if (nLen > SNDBUFFER_BLOCK_SIZE)
		{
			memcpy(m_szBlock, pszData, SNDBUFFER_BLOCK_SIZE);
			m_nDataLen = SNDBUFFER_BLOCK_SIZE;
		}
		else
		{
			memcpy(m_szBlock, pszData, nLen);
			m_nDataLen = nLen;
		}
		return m_nDataLen;
	}
	void sSndBufferBlock::OnSend(int nLength)
	{
		if (nLength >= m_nDataLen)
		{
			m_nDataLen = 0;
			return;
		}
		m_nDataLen -= nLength;
		memcpy(m_szBlock, m_szBlock + nLength, m_nDataLen);
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	CSendBuffer::CSendBuffer(void) : m_poolBlock("sSndBufferBlock")
	{
		m_pBlockSending = NULL;
		m_nQueueLenMax = 0;
	}

	CSendBuffer::~CSendBuffer(void)
	{}

	BOOL CSendBuffer::IsQueueFulled()
	{
		if (m_nQueueLenMax <= 0)
			return FALSE;
		return (GetQueueLen() > m_nQueueLenMax);
	}

	sSndBufferBlock* CSendBuffer::AllocateBlock()
	{
		return m_poolBlock.ApplyObj();
	}
	void CSendBuffer::ReleaseBlock(sSndBufferBlock* pObj)
	{
		m_poolBlock.ReclaimObj(pObj);
	}

	BOOL CSendBuffer::IsSending()
	{
		return (m_pBlockSending != NULL);
	}
	///向缓冲区中增加一段内容，
	BOOL CSendBuffer::AddBuffer(const char* buf, int nSize)
	{
		int nWrote = 0;
		if (m_pBlockSending == NULL)
		{
			m_pBlockSending = AllocateBlock();
			if (m_pBlockSending == NULL)
			{
				LOG("Allocate sSndBufferBlock Failed : %d", GetLastError());
				return FALSE;
			}
			nWrote += m_pBlockSending->SetData(buf + nWrote, nSize - nWrote);
		}
		else if (IsQueueFulled())
		{
			LOG("The Queue Is Fulled CurQueue=%d MaxQueue=%d", GetQueueLen(), m_nQueueLenMax);
			return FALSE;
		}
		while (nWrote < nSize)
		{
			sSndBufferBlock* pBlock = AllocateBlock();
			if (pBlock == NULL)
			{
				LOG("Allocate sSndBufferBlock Failed : %d", GetLastError());
				return FALSE;
			}
			nWrote += pBlock->SetData(buf + nWrote, nSize - nWrote);
			m_queueSnd.push_back(pBlock);
		}
		return TRUE;
	}
	char* CSendBuffer::GetDataToSend()
	{
		if (m_pBlockSending)
			return m_pBlockSending->GetBlock();
		return NULL;
	}
	int CSendBuffer::GetDataLenToSend()
	{
		if (m_pBlockSending)
			return m_pBlockSending->GetDataLen();
		return 0;
	}
	BOOL CSendBuffer::OnSend(int nLength)
	{	//仍有数据要发送，返回TRUE，否则返回FALSE
		if (m_pBlockSending)
		{
			m_pBlockSending->OnSend(nLength);
			if (m_pBlockSending->GetDataLen() > 0)
				return TRUE;
			ReleaseBlock(m_pBlockSending);
			m_pBlockSending = NULL;
		}
		while (m_pBlockSending == NULL && m_queueSnd.size() > 0)
		{
			m_pBlockSending = m_queueSnd.pop_front();
			if (m_pBlockSending->GetDataLen() <= 0)
			{
				ReleaseBlock(m_pBlockSending);
				m_pBlockSending = NULL;
			}
		}
		if (m_pBlockSending)
			return TRUE;
		return FALSE;
	}
	void CSendBuffer::Clear()
	{
		if (m_pBlockSending)
		{
			ReleaseBlock(m_pBlockSending);
			m_pBlockSending = NULL;
		}
		while (m_queueSnd.size() > 0)
		{
			sSndBufferBlock* pBlock = m_queueSnd.pop_front();
			if (pBlock)
			{
				ReleaseBlock(pBlock);
				pBlock = NULL;
			}
		}
	}
	int CSendBuffer::GetQueueLen()
	{
		return m_queueSnd.size();
	}
	void CSendBuffer::SetQueueLenMax(int nMax)
	{
		m_nQueueLenMax = nMax;
	}
}