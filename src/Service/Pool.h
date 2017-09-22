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

#include "SyncObj.h"

#define NEW_OBJECTPOOL

namespace YTSvrLib
{
	// 对象池基类
	class YTSVRLIB_EXPORT CRecycle
	{
	public:
		CRecycle()
		{
			m_bInUse = false;
		}

		virtual void Init()
		{};
		virtual void Clear()
		{};

		bool InUse() const
		{
			return m_bInUse;
		}
		void SetInUse(bool bInUse)
		{
			m_bInUse = bInUse;
		}

		void SetIdx(const std::pair< INT_PTR, INT_PTR >& nIndex)
		{
			m_Idx = nIndex;
		}

		const std::pair< INT_PTR, INT_PTR >& GetIdx()
		{
			return m_Idx;
		}

	private:

		std::pair< INT_PTR, INT_PTR >    m_Idx;
		bool                            m_bInUse;
	};

#ifdef NEW_OBJECTPOOL

	template< typename ObjectType, long SIZE>
	class CPool
	{
	public:
		explicit CPool(LPCSTR pszObjName)
		{
			Init(pszObjName);
		}

		virtual ~CPool()
		{
			for_each(m_listFreeObject.begin(), m_listFreeObject.end(), Destroy);
		}

		inline static void Destroy(ObjectType* obj)
		{
			delete obj;
		}

		inline static ObjectType* Construct()
		{
			ObjectType* pObj = new ObjectType;
			if (pObj)
			{
				pObj->SetInUse(false);
			}

			return pObj;
		}

		void Init(LPCSTR pszObjName)
		{
			for (long i = 0; i < SIZE; ++i)
			{
				m_listFreeObject.push_back(Construct());
			}
			m_bLogEnable = TRUE;
			strncpy_s(m_szObjType, pszObjName, 32);
			m_szObjType[32] = '\0';
			m_nTotalCount = m_listFreeObject.size();
		}

		void SetLogEnable(BOOL bEnable)
		{
			m_bLogEnable = bEnable;
		}
		LPCSTR GetObjName()
		{
			return m_szObjType;
		}

		size_t GetTotalCount()
		{
			return m_nTotalCount;
		}

		void ConstructObject()
		{
			try
			{
				for (long i = 0; i < SIZE; ++i)
				{
					m_listFreeObject.push_back(Construct());
				}
				m_nTotalCount += SIZE;
				LOGTRACE("Pool=0x%x Construct Object %s.Cur Total Count : %d", this, GetObjName(), GetTotalCount());
			}
			catch (...)
			{
				LOGERROR("Memory Overflow @ Construct %s", GetObjName());
				return;
			}
		}

		ObjectType* ApplyObj()
		{
			ObjectType* Obj = NULL;

			m_Lock.Lock();

			if (m_listFreeObject.empty())
			{
				ConstructObject();
			}

			while (Obj == NULL && m_listFreeObject.size() > 0)
			{
				Obj = m_listFreeObject.front();

				m_listFreeObject.pop_front();

				if (Obj->InUse())
				{
					Obj = NULL;
					continue;
				}

				Obj->SetInUse(true);
			}

			m_Lock.UnLock();

			if (Obj == NULL)
			{
				if (m_bLogEnable)
					LOG("Pool=0x%x %s Allocate Error=%d!", this, GetObjName(), GetLastError());
			}
			else
				Obj->Init();

			return Obj;
		}

		void ReclaimObj(ObjectType* pObj)
		{
			if (pObj == NULL)
			{
				if (m_bLogEnable)
				{
					LOG("Reclaim Object %s Obj==NULL", GetObjName());
				}
				return;
			}
			if (!pObj->InUse())
			{
				if (m_bLogEnable)
					LOG("Pool=0x%x %s Obj=0x%08x Not in use when free.", this, GetObjName(), pObj);
				return;
			}

			m_Lock.Lock();

			pObj->SetInUse(false);
			pObj->Init();

			m_listFreeObject.push_back(pObj);

			if (m_listFreeObject.size() >= (SIZE * 2))
			{
				while (m_listFreeObject.size() >= SIZE)
				{
					ObjectType* pCurObj = m_listFreeObject.back();
					m_listFreeObject.pop_back();
					if (pCurObj)
					{
						delete pCurObj;
					}
					m_nTotalCount--;
				}

				LOGTRACE("Pool=0x%x Free Object %s.Cur Total Count : %d", this, GetObjName(), GetTotalCount());
			}

			m_Lock.UnLock();
		}
	private:
		std::list<ObjectType*> m_listFreeObject;
#ifdef LIB_WINDOWS
		YTSvrLib::CLock                             m_Lock;
#else
		YTSvrLib::CRecursiveLock                    m_Lock;
#endif // LIB_WINDOWS
		BOOL										m_bLogEnable;
		char m_szObjType[33];
		size_t m_nTotalCount;// 当前对象池总共分配出去的数目
	};

#else
	template< typename ObjectType, long SIZE, long MINSIZE = 8>
	class CPool
	{
		typedef struct __PoolPage
		{
			std::vector< ObjectType* >   m_vctObjPool;
			std::vector<INT_PTR> m_vctFreeIdx;
			size_t m_nID;
			__PoolPage(size_t nID) : m_nID(nID), m_vctObjPool(SIZE)
			{
				for (INT_PTR i = 0; i < SIZE; ++i)
				{
					m_vctObjPool[i] = new ObjectType;
					if (m_vctObjPool[i])
					{
						m_vctObjPool[i]->SetInUse(false);
						m_vctObjPool[i]->SetIdx(std::make_pair(m_nID, i));
						m_vctFreeIdx.push_back(i);
					}
				}
			}
			~__PoolPage()
			{
				for (INT_PTR i = 0; i < SIZE; ++i)
				{
					if (m_vctObjPool[i])
						delete m_vctObjPool[i];
				}
			}
			BOOL IsInUse()
			{
				return (m_vctFreeIdx.size() < m_vctObjPool.size());
			}
		}PoolPage;

	public:

		CPool(LPCSTR pszObjName)
		{
			Init(pszObjName);
		}
		~CPool()
		{
			for (INT_PTR i = 0; i < (INT_PTR) m_vctPoolPage.size(); ++i)
			{
				if (m_vctPoolPage[i])
					delete m_vctPoolPage[i];
			}
			m_vctPoolPage.clear();
			//m_vctFreeIdx.clear();
		}

		void Init(LPCSTR pszObjName)
		{
			m_nLastPageID = 0;
			m_vctPoolPage.push_back(new PoolPage(m_nLastPageID));
			m_nPageCount = 1;
			m_dwSleepTime = 500;
			m_bLogEnable = TRUE;
			strncpy_s(m_szObjType, pszObjName, 32);
			m_szObjType[32] = '\0';
#ifdef _DEBUG
			if (strcmp(m_szObjType, "CLogManager") != 0)
				LOG("CPool=0x%08x Obj=%s size=%d", this, m_szObjType, sizeof(ObjectType));
#endif
		}

		//    bool ReachMax(){ return m_vctFreeIdx.size() < 10; }
		size_t GetPages()
		{
			return m_vctPoolPage.size();
		}
		size_t GetTotalCount()
		{
			return m_vctPoolPage.size()*SIZE;
		}
		//	UINT GetFreeCount() { return m_vctFreeIdx.size(); }
		void SetLogEnable(BOOL bEnable)
		{
			m_bLogEnable = bEnable;
		}
		LPCSTR GetObjName()
		{
			return m_szObjType;
		}

		ObjectType* ApplyObj()
		{
			// #ifdef _DEBUG
			// 		ObjectType* pObject = 0;
			// 		pObject = new ObjectType;
			// 		if( pObject )
			// 		{
			// 			pObject->SetInUse( true );
			// 			pObject->Init();
			// 	//		if( sizeof(ObjectType) == 99 )
			// 				LOG("CPool_ApplyObj Obj=%s Addr=0x%08x", m_szObjType, pObject );
			// 		}
			// 		return pObject;
			// #else
			ObjectType* pObject = 0;
#ifndef LIB_WINDOWS
			//		printf("Now ApplyObj = %s\n", GetObjName());
#endif // !LIB_WINDOWS
			m_Lock.Lock();
			size_t p;
			for (p = 0; p<m_vctPoolPage.size(); p++)
			{
				if (m_vctPoolPage[p] && m_vctPoolPage[p]->m_vctFreeIdx.size() > 0)
					break;
			}
			if (p >= m_vctPoolPage.size())
			{
				for (p = 0; p<m_vctPoolPage.size(); p++)
					if (m_vctPoolPage[p] == NULL)
						break;
				PoolPage* pNewPage = new PoolPage(p);
				if (pNewPage)
				{
					if (p < m_vctPoolPage.size())
					{
						m_vctPoolPage[p] = pNewPage;
						if (p > m_nLastPageID)
							m_nLastPageID = p;
					}
					else
					{
						m_vctPoolPage.push_back(pNewPage);
						m_nLastPageID = m_vctPoolPage.size() - 1;
						//p = m_nLastPageID;
					}
					m_nPageCount++;
				}
				LOG("Pool=0x%08x %s Allocate=%d*%d Page=%d Total=%d/%d", this, GetObjName(), sizeof(ObjectType), SIZE, pNewPage->m_nID, m_nPageCount, m_vctPoolPage.size());
			}
			while (p < m_vctPoolPage.size())
			{
				if (m_vctPoolPage[p] && m_vctPoolPage[p]->m_vctFreeIdx.size() > 0)
				{
					size_t nIdx = m_vctPoolPage[p]->m_vctFreeIdx.back();
					m_vctPoolPage[p]->m_vctFreeIdx.pop_back();
					while (nIdx >= m_vctPoolPage[p]->m_vctObjPool.size())
					{
						if (m_vctPoolPage[p]->m_vctFreeIdx.empty())
							break;
						nIdx = m_vctPoolPage[p]->m_vctFreeIdx.back();
						m_vctPoolPage[p]->m_vctFreeIdx.pop_back();
					}
					if (nIdx >= m_vctPoolPage[p]->m_vctObjPool.size()
						|| m_vctPoolPage[p]->m_vctObjPool[nIdx] == NULL
						|| m_vctPoolPage[p]->m_vctObjPool[nIdx]->InUse())
					{
						p++;
						continue;
					}
					pObject = (m_vctPoolPage[p]->m_vctObjPool[nIdx]);
					pObject->SetInUse(true);
					//pObject->SetIdx( std::make_pair( p, nIdx ) );        
					break;
				}
				p++;
			}
			m_Lock.UnLock();
			if (pObject == NULL)
			{
				if (m_bLogEnable)
					LOG("Pool=0x%08x %s Allocate Error=%d!", this, GetObjName(), GetLastError());
			}
			else
				pObject->Init();
			return pObject;
		}

		void ReclaimObj(ObjectType* pObj)
		{
			// #ifdef _DEBUG
			// 		// 		if( m_bLogEnable == TRUE )
			// 		// 			LOG("Pool=0x%08x %s ReclaimObj=0x%08x Size=%d", this, GetObjName(), pObj, sizeof(ObjectType));
			// 		pObj->SetInUse(false);
			// 		delete pObj;
			// 		return;
			// #else
			m_Lock.Lock();
			if (!pObj->InUse())
			{
				if (m_bLogEnable)
					LOG("Pool=0x%08x %s Obj=0x%08x Not in use when free.", this, m_szObjType, pObj);
				m_Lock.UnLock();
				return;
			}
			pObj->SetInUse(false);
			pObj->Init();
			/*if( std::find( m_vctFreeIdx.begin(), m_vctFreeIdx.end(), pObj->GetIdx() ) != m_vctFreeIdx.end() )
			{
			LOG("ReclaimObj error!");
			}*/
			const std::pair<INT_PTR, INT_PTR>& nIdx = pObj->GetIdx();
			if ((size_t) (nIdx.first) < m_vctPoolPage.size()
				&& (size_t) (nIdx.second) < SIZE && m_vctPoolPage[nIdx.first] != NULL)
			{
				size_t nPageID = nIdx.first;
				m_vctPoolPage[nPageID]->m_vctFreeIdx.push_back((UINT) (nIdx.second));
				//m_vctFreeIdx.push_back( pObj->GetIdx() );
				while (m_nPageCount > MINSIZE && nPageID == m_nLastPageID
					   && m_vctPoolPage[nPageID]
					   && m_vctPoolPage[nPageID]->IsInUse() == false)
				{
					size_t i;
					for (i = 0; i<m_vctPoolPage[nPageID]->m_vctObjPool.size(); i++)
						if (m_vctPoolPage[nPageID]->m_vctObjPool[i]
							&& m_vctPoolPage[nPageID]->m_vctObjPool[i]->InUse())
							break;
					if (i < m_vctPoolPage[nPageID]->m_vctObjPool.size())
						break;
					m_nPageCount--;
					LOG("Pool=0x%08x %s Release Page=%d Total=%d/%d", this, GetObjName(), m_vctPoolPage[nPageID]->m_nID, m_nPageCount, m_vctPoolPage.size());
					delete m_vctPoolPage[nPageID];
					m_vctPoolPage[nPageID] = NULL;
					m_nLastPageID--;
					nPageID--;
				}
			}
			else
				if (m_bLogEnable)
					LOG("Pool=0x%08x %s ReclaimObj=0x%08x Invalid Idx=%d,%d", this, m_szObjType, pObj, nIdx.first, nIdx.second);
			m_Lock.UnLock();
			//#endif
		}

	private:

		DWORD                                       m_dwSleepTime;
#ifdef LIB_WINDOWS
		YTSvrLib::CLock                             m_Lock;
#else
		YTSvrLib::CRecursiveLock                    m_Lock;
#endif // LIB_WINDOWS
		std::vector< PoolPage* >                    m_vctPoolPage;
		size_t										m_nPageCount;
		size_t										m_nLastPageID;
		//std::vector< std::pair<INT_PTR,INT_PTR> >   m_vctFreeIdx;
		BOOL										m_bLogEnable;
		char m_szObjType[33];
	};

#endif // NEW_OBJECTPOOL
}
