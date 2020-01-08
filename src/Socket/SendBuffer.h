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
#define BLOCK_RECV_BUFFER_SIZE	(1024 X16)

namespace YTSvrLib
{
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

	class CBuffer
	{
	public:
		CBuffer();

		const char* GetBuffer();

		std::string& GetRecvingBuffer();

		void AddBuffer(const char* buf, size_t len);

		int GetLength();

		void ReleaseBuffer(size_t nRead);

		void Clear();

		void ReSize(size_t nNewSize = 0);
	private:
		std::string _curRecving;
		std::string _buffer;
		size_t _recvingBuffSize;
	};
}