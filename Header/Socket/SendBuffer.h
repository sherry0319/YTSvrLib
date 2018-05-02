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

#pragma warning( push )
#pragma warning(disable:4273)
#pragma warning(disable:4100)
#include "libwebsockets.h"
#pragma warning( pop )

#define SNDBUFFER_BLOCK_SIZE	1024

namespace YTSvrLib
{
	struct YTSVRLIB_EXPORT WSSndBuffBlock : public CRecycle
	{
		std::string m_msg;
		lws_write_protocol m_type;

		virtual void Init()
		{
			m_msg.clear();
			m_msg.shrink_to_fit();
			m_type = LWS_WRITE_TEXT;
		}

		std::string& GetMsg()
		{
			return m_msg;
		}

		const char* GetMsgData()
		{// 用libwebsockets发送者要自己预留出填充帧头部信息的空间.见:https://libwebsockets.org/lws-api-doc-master/html/group__sending-data.html 中的 IMPORTANT NOTICE!
			return (m_msg.c_str() + LWS_PRE);
		}

		int GetMsgLen()
		{// 用libwebsockets发送者要自己预留出填充帧头部信息的空间.见:https://libwebsockets.org/lws-api-doc-master/html/group__sending-data.html 中的 IMPORTANT NOTICE!
			return (int) (m_msg.size() - LWS_PRE);
		}

		void SetMsg(const char* msg, int len, lws_write_protocol type)
		{
			std::string prefix;
			prefix.assign(LWS_PRE, '\0');
			std::string real;
			real.assign(msg, len);

			m_msg = prefix + real;
			m_type = type;
		}
	};

	class YTSVRLIB_EXPORT CWSSendBuffer
	{
	public:
		CWSSendBuffer();
		virtual ~CWSSendBuffer();

	public:
		WSSndBuffBlock* AllocateBlock();
		void ReleaseBlock(WSSndBuffBlock* pObj);

		BOOL IsSending();

		BOOL AddBlock(const char* buf, int len, lws_write_protocol type);

		const char* GetDataToSend();

		int GetDataLenToSend();

		lws_write_protocol GetDataTypeToSend();

		BOOL OnSend();

		void Clear();

		int GetQueueLen();

		void SetQueueLenMax(int nMax);

		BOOL IsQueueFulled();
	private:
		WSSndBuffBlock* m_pBlockSending;
		CWQueue<WSSndBuffBlock*> m_queueSnd;
		int m_nQueueLenMax;
		CPool<WSSndBuffBlock, 64> m_poolBlock;
	};

	//////////////////////////////////////////////////////////////////////////

	struct YTSVRLIB_EXPORT sSndBufferBlock : public CRecycle
	{
		char m_szBlock[SNDBUFFER_BLOCK_SIZE];
		int m_nDataLen;

		virtual void Init();

		char* GetBlock();

		int GetDataLen();

		int SetData(LPCSTR pszData, int nLen);

		void OnSend(int nLength);
	};

	class YTSVRLIB_EXPORT CSendBuffer
	{
	public:
		CSendBuffer(void);
		virtual ~CSendBuffer(void);

		sSndBufferBlock* AllocateBlock();

		void ReleaseBlock(sSndBufferBlock* pObj);

		BOOL IsSending();

		///向缓冲区中增加一段内容，
		BOOL AddBuffer(const char* buf, int nSize);

		char* GetDataToSend();

		int GetDataLenToSend();

		BOOL OnSend(int nLength);

		void Clear();

		int GetQueueLen();

		void SetQueueLenMax(int nMax);

		BOOL IsQueueFulled();

	private:
		sSndBufferBlock* m_pBlockSending;
		CWQueue<sSndBufferBlock*> m_queueSnd;
		int m_nQueueLenMax;
		CPool<sSndBufferBlock, 64> m_poolBlock;
	};

	//////////////////////////////////////////////////////////////////////////

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