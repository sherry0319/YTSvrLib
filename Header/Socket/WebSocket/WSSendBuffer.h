#pragma once

#ifndef __WSSEND_BUFFER_H_
#define __WSSEND_BUFFER_H_

namespace YTSvrLib {
	struct YTSVRLIB_EXPORT WSSndBuffBlock : public CRecycle
	{
		std::string m_msg;

		virtual void Init()
		{
			m_msg.clear();
			m_msg.shrink_to_fit();
		}

		std::string& GetMsg()
		{
			return m_msg;
		}

		const char* GetMsgData()
		{// 用libwebsockets发送者要自己预留出填充帧头部信息的空间.见:https://libwebsockets.org/lws-api-doc-master/html/group__sending-data.html 中的 IMPORTANT NOTICE!
			return (m_msg.c_str());
		}

		int GetMsgLen()
		{// 用libwebsockets发送者要自己预留出填充帧头部信息的空间.见:https://libwebsockets.org/lws-api-doc-master/html/group__sending-data.html 中的 IMPORTANT NOTICE!
			return (int)(m_msg.size());
		}

		void SetMsg(const char* msg, int len)
		{
			m_msg.assign(msg, len);
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

		BOOL AddBlock(const char* buf, int len);

		const char* GetDataToSend();

		int GetDataLenToSend();

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
}

#endif