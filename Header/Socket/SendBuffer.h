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
#pragma once
#include "../stl/wqueue.h"

#define SNDBUFFER_BLOCK_SIZE	1024

namespace YTSvrLib
{
	struct sSndBufferBlock : public CRecycle
	{
		char m_szBlock[SNDBUFFER_BLOCK_SIZE];
		int m_nDataLen;

		virtual void Init()
		{
			m_nDataLen = 0;
		}
		char* GetBlock()
		{
			return m_szBlock;
		}
		int GetDataLen()
		{
			return m_nDataLen;
		}
		int SetData(LPCSTR pszData, int nLen)
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
		void OnSend(int nLength)
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
	};

	class CSendBuffer
	{
	public:
		CSendBuffer(void);
		~CSendBuffer(void);

		sSndBufferBlock* AllocateBlock()
		{
			return m_poolBlock.ApplyObj();
		}
		void ReleaseBlock(sSndBufferBlock* pObj)
		{
			m_poolBlock.ReclaimObj(pObj);
		}

		BOOL IsSending()
		{
			return (m_pBlockSending != NULL);
		}
		///向缓冲区中增加一段内容，
		BOOL AddBuffer(const char* buf, int nSize)
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
		char* GetDataToSend()
		{
			if (m_pBlockSending)
				return m_pBlockSending->GetBlock();
			return NULL;
		}
		int GetDataLenToSend()
		{
			if (m_pBlockSending)
				return m_pBlockSending->GetDataLen();
			return 0;
		}
		BOOL OnSend(int nLength)
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
		void Clear()
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
		int GetQueueLen()
		{
			return m_queueSnd.size();
		}
		void SetQueueLenMax(int nMax)
		{
			m_nQueueLenMax = nMax;
		}
		BOOL IsQueueFulled();

	private:
		sSndBufferBlock* m_pBlockSending;
		CWQueue<sSndBufferBlock*> m_queueSnd;
		int m_nQueueLenMax;
		CPool<sSndBufferBlock, 64> m_poolBlock;
	};

#ifdef LIB_WINDOWS
	class CWinsock
	{
	public:
		explicit CWinsock(BYTE mVers = '2', BYTE sVers = '2')
		{
			WSADATA      wsd;

			::WSAStartup(MAKEWORD(mVers, sVers), &wsd);
		}
		virtual ~CWinsock(void)
		{
			::WSACleanup();
		}
	};
#endif // LIB_WINDOWS

	template<size_t size>
	class CBuffer
	{
	public:
		//CBuffer(const CBuffer& rSrc){  }
		//void operator=(const CBuffer& rSrc);
		CBuffer() : m_nLength(0)
		{
			m_nBufSize = size;
			m_pbuf = new char[size];
			ZeroMemory(m_pbuf, size);
			m_nBufSizeMax = size * 16;
		}
		~CBuffer()
		{
			if (m_pbuf)
			{
				delete[] m_pbuf;
				m_pbuf = NULL;
			}
			m_nBufSize = 0;
		}
		CBuffer<size>(CBuffer<size>& other) : m_nLength(other.GetLength())
		{
			m_nBufSize = other.GetCapacity();
			m_pbuf = new char[m_nBufSize];
			memcpy(m_pbuf, other.GetBuffer(), other.GetLength());
			m_nBufSizeMax = other.GetBufSizeMax();
			m_nLength = other.GetLength();
		}

		///获取缓冲区的总容量
		size_t GetCapacity()
		{
			return m_nBufSize;
		}
		void SetBufSizeMax(size_t nMax)
		{
			m_nBufSizeMax = nMax;
		}
		size_t GetBufSizeMax()
		{
			return m_nBufSizeMax;
		}
		BOOL ReSize(size_t nNewSize)
		{
			if (nNewSize < m_nLength)
				return FALSE;
			char* pNewBuf = new char[nNewSize];
			if (pNewBuf == NULL)
				return FALSE;
			ZeroMemory(pNewBuf, nNewSize);
			if (m_pbuf)
			{
				if (m_nLength > 0)
					memcpy(pNewBuf, m_pbuf, m_nLength);
				delete[] m_pbuf;
			}
			else
				m_nLength = 0;
			m_pbuf = pNewBuf;
			m_nBufSize = nNewSize;
			if (m_nBufSize > m_nBufSizeMax)
				m_nBufSizeMax = m_nBufSize;
			return TRUE;
		}

		///获取当前缓冲区中已填充的内容的长度
		size_t GetLength()
		{
			return m_nLength;
		}

		char * GetIdleBuffer()
		{
			return m_pbuf + m_nLength;
		}

		size_t GetIdleLength()
		{
			return m_nBufSize - m_nLength;
		}

		///向缓冲区中增加一段内容，若增加后缓冲区将越界，则撤销增加并返回失败
		BOOL AddBuffer(const char* buf, size_t nSize)
		{
			if (m_nLength + nSize > m_nBufSize)
			{
				size_t nNeedSizeMin = m_nLength + nSize;
				size_t nNewBufSize = m_nBufSize + ((nNeedSizeMin - m_nBufSize) / size + 1)*size;
				if (nNewBufSize > m_nBufSizeMax)
					return FALSE;
				if (FALSE == ReSize(nNewBufSize))
					return FALSE;
			}
			memcpy((unsigned char*) &m_pbuf[m_nLength], buf, nSize);
			m_nLength += nSize;
			return TRUE;
		}

		//增加缓冲区中内容的长度
		BOOL AddBuffer(size_t nSize)
		{
			if (m_nLength + nSize > m_nBufSize)
			{
				return FALSE;
			}
			m_nLength += nSize;
			return TRUE;
		}

		///获取缓冲区头的地址
		char* GetBuffer()
		{
			return m_pbuf;
		}

		///释放从缓冲区开始到指定长度的内容空间
		void ReleaseBuffer(size_t nLength)
		{
			if (nLength >= m_nLength)
			{
				m_nLength = 0;
			}
			else
			{
				if (nLength > 0)
				{
					m_nLength -= nLength;
					memcpy(&m_pbuf[0], &m_pbuf[nLength], m_nLength);
				}
				else
				{
					// 如果用户没有处理掉任何数据，同时缓冲区已经满了，清空缓冲
					if (m_nLength >= m_nBufSize)
					{
						m_nLength = 0;
					}
				}
			}
		}

		///恢复初始状态
		void Clear()
		{
			m_nLength = 0;
		}
	private:
		size_t m_nLength;
		size_t m_nBufSize;
		size_t m_nBufSizeMax;
		char* m_pbuf;
	};
}