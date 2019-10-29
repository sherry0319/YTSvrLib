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
namespace YTSvrLib
{
	enum EM_MESSAGE_TYPE {
		MSGTYPE_NULL,
		MSGTYPE_DATA,
		MSGTYPE_DISCONNECT = 10,
		MSGTYPE_CONNECTED,
		MSGTYPE_CONNECTFAILED,
		MSGTYPE_ACCEPTED,
	};
	
	template<class CONNTYPE>
	struct YTSVRLIB_EXPORT MSGITEM : public CRecycle
	{
		MSGITEM() {
			Clean();
		}

		virtual void Init() {
			Clean();
		}

		void Clean() {
			m_emType = MSGTYPE_NULL;
			m_pConn = NULL;
			m_data.clear();
			m_data.shrink_to_fit();
		}

		EM_MESSAGE_TYPE m_emType;
		CONNTYPE* m_pConn;
		std::string m_data;
	};

	template<class CONNTYPE>
	class YTSVRLIB_EXPORT CMessageQueue {
		typedef MSGITEM<CONNTYPE> MSG;
	public:
		CMessageQueue():m_poolMsgs("MSGITEM") {
			m_queueMsgs.clear();
		}
		
		virtual void SetEvent() = 0;
	public:
		void AddNewMessage(EM_MESSAGE_TYPE emType, CONNTYPE* pConn, const char* data = NULL, int len = 0) {
			auto* pNewItem = m_poolMsgs.ApplyObj();
			if (pNewItem)
			{
				pNewItem->m_emType = emType;
				pNewItem->m_pConn = pConn;
				if (data && len > 0)
				{
					pNewItem->m_data.assign(data, len);
				}

				BOOL bSetEvent = FALSE;
				m_lockMsg.Lock();
				if (m_queueMsgs.empty())
				{
					bSetEvent = TRUE;
				}
				m_queueMsgs.push_back(pNewItem);
				m_lockMsg.UnLock();

				if (bSetEvent)
				{
					SetEvent();
				}
			}
		}

		int GetQueueSize() {
			return (int)m_queueMsgs.size();
		}
	protected:
		virtual void MessageConsumer() {
			if (m_queueMsgs.empty())
			{
				return;
			}

			int nCount = 0;
			BOOL bNeedSetEvt = FALSE;
			do
			{
				if (nCount >= 128)
				{	//避免其他事件队列堵塞
					bNeedSetEvt = TRUE;
					break;
				}

				m_lockMsg.Lock();
				if (m_queueMsgs.empty())
				{
					m_lockMsg.UnLock();
					break;
				}
				MSG* pItem = m_queueMsgs.front();
				m_queueMsgs.pop_front();
				m_lockMsg.UnLock();

				if (pItem)
				{
					switch (pItem->m_emType)
					{
					case MSGTYPE_DATA: {
						ProcessMessage(pItem->m_pConn, pItem->m_data.data(), (int)pItem->m_data.size());
					}break;
					case MSGTYPE_DISCONNECT: {
						ProcessDisconnectEvent(pItem->m_pConn);
					}break;
					case MSGTYPE_ACCEPTED:
					case MSGTYPE_CONNECTED:
					case MSGTYPE_CONNECTFAILED: {
						ProcessEvent(pItem->m_emType, pItem->m_pConn);
					}break;
					default:
						break;
					}
				}

				m_poolMsgs.ReclaimObj(pItem);
			} while (true);

			if (bNeedSetEvt)
			{
				SetEvent();
			}
		}
		
		// 处理数据消息
		virtual void ProcessMessage(CONNTYPE* pConn, const char* data, int len) = 0;

		// 处理事件消息
		virtual void ProcessEvent(EM_MESSAGE_TYPE emType, CONNTYPE* pConn) = 0;

	private:
		// 处理断开事件消息
		virtual void ProcessDisconnectEvent(CONNTYPE* pConn) {
			ProcessEvent(MSGTYPE_DISCONNECT, pConn);
		}
	private:
		std::list<MSG*> m_queueMsgs;
		CPool<MSG,128> m_poolMsgs;
		CLock m_lockMsg;
	};
}