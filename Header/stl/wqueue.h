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
#ifndef _W_QUEUE_H
#define _W_QUEUE_H
#include "../Service/Utility.h"
#include "../Global/GlobalServer.h"

namespace YTSvrLib
{

	template<class T>
	class CWQueue
	{
		struct _WQueue_Node : public CRecycle
		{
			T _value;
			struct _WQueue_Node*	_pNext;
			virtual void Init()
			{
				_pNext = NULL;
			}
		};
	public:
		CWQueue() : m_poolNode("_WQueue_Node")
		{
			m_pHeadNode = NULL;
			m_pTailNode = NULL;
			m_nQueueLen = 0;
		}
		~CWQueue()
		{
			Lock();
			while (m_pHeadNode)
			{
				_WQueue_Node* pNode = m_pHeadNode;
				m_pHeadNode = pNode->_pNext;
				ReleaseNode(pNode);
			}

			UnLock();
		}

		void Lock()
		{
			m_Lock.Lock();
		}
		void UnLock()
		{
			return m_Lock.UnLock();
		}

		bool empty() const
		{
			return (m_pHeadNode == NULL);
		}
		UINT	size()
		{
			return m_nQueueLen;
		}
		void push_front(const T& _Val)
		{
			_WQueue_Node* pNode = AllocateNode();
			if (pNode == NULL)
			{
				LOG("WQueue_pushfront allocate size=%d Error=%d", sizeof(T), GetLastError());
				return;
			}
			pNode->_value = _Val;
			if (m_pHeadNode == NULL)
			{
				pNode->_pNext = NULL;
				m_pHeadNode = pNode;
				m_pTailNode = pNode;
			}
			else
			{
				pNode->_pNext = m_pHeadNode;
				m_pHeadNode = pNode;
			}
			m_nQueueLen++;
		}
		void push_back(const T& _Val)
		{
			_WQueue_Node* pNode = AllocateNode();
			if (pNode == NULL)
			{
				LOG("WQueue_pushback allocate size=%d Error=%d", sizeof(T), GetLastError());
				return;
			}
			pNode->_value = _Val;
			pNode->_pNext = NULL;
			if (m_pTailNode == NULL)
			{
				m_pHeadNode = pNode;
				m_pTailNode = pNode;
			}
			else
			{
				m_pTailNode->_pNext = pNode;
				m_pTailNode = pNode;
			}
			m_nQueueLen++;
		}
		T	pop_front()
		{
			_ASSERT(m_pHeadNode != NULL);
			if (m_pHeadNode == NULL)
				return NULL;
			_WQueue_Node* pNode = m_pHeadNode;
			m_pHeadNode = pNode->_pNext;
			if (m_pHeadNode == NULL)
			{
				m_pTailNode = NULL;
				m_nQueueLen = 0;
			}
			else
				m_nQueueLen--;
			T _Val = pNode->_value;
			ReleaseNode(pNode);
			return _Val;
		}
		void erase(const T& _Val)
		{
			if (m_pHeadNode == NULL)
				return;
			if (m_pHeadNode->_value == _Val)
			{
				_WQueue_Node* pNode = m_pHeadNode;
				m_pHeadNode = m_pHeadNode->_pNext;
				if (m_pHeadNode == NULL)
				{
					m_pTailNode = NULL;
					m_nQueueLen = 0;
				}
				else
					m_nQueueLen--;
				ReleaseNode(pNode);
				return;
			}
			_WQueue_Node* pNode = m_pHeadNode;
			while (pNode->_pNext)
			{
				if (pNode->_pNext->_value == _Val)
				{
					_WQueue_Node* pDel = pNode->_pNext;
					pNode->_pNext = pDel->_pNext;
					if (pNode->_pNext == NULL)
						m_pTailNode = pNode;
					m_nQueueLen--;
					ReleaseNode(pDel);
					return;
				}
				pNode = pNode->_pNext;
			}
		}
	protected:
		_WQueue_Node* AllocateNode()
		{
			_WQueue_Node* pNodeNew = m_poolNode.ApplyObj();

			return pNodeNew;
		}
		void	ReleaseNode(_WQueue_Node* pNode)
		{
			m_poolNode.ReclaimObj(pNode);
		}
	private:
		YTSvrLib::CLock           m_Lock;
		CPool<_WQueue_Node, 128> m_poolNode;
		_WQueue_Node*	m_pHeadNode;
		_WQueue_Node*	m_pTailNode;
		UINT			m_nQueueLen;
	};
}

#endif //_W_QUEUE_H