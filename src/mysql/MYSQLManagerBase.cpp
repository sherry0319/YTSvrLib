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
#include "MYSQLManagerBase.h"
//解析db返回数据,在CallBack中调用

namespace YTSvrLib
{
	void CMYSQLManagerBase::ParseResponse(UINT nErrorCode, ULONG nAffectRows, void* dwKey, MYSQLLIB::CMYSQLRecordSet* pResSet, BOOL bAsync)
	{
		if (dwKey == NULL)
		{

			LOG("CMYSQLManagerBase::ParseResponse Database Handle Failed");
			if (pResSet)
			{
				m_DBSys.ReclaimRecSet(pResSet);
			}
			return;
		}
		MYSQLRESPONSEINFO* pResponseInfo = ApplyRespInfo();
		pResponseInfo->nErrorCode = nErrorCode;
		pResponseInfo->nAffectRows = nAffectRows;
		pResponseInfo->nKey = dwKey;
		pResponseInfo->pResSet = pResSet;

		if (bAsync)
		{
			BOOL bMustSetEvent = FALSE;//是否需要触发事件
			m_qResponse.Lock();
			if (m_qResponse.empty())
			{
				bMustSetEvent = TRUE;
			}
			m_qResponse.push_back(pResponseInfo);
#ifdef COREDEBUG
			LOG("ParseResponse = %d", m_qResponse.size());
#endif
			m_qResponse.UnLock();
			if (bMustSetEvent)
			{
#ifdef COREDEBUG
				LOG("ParseResponse = SetEvent()");
#endif
				SetEvent();
			}
		}
		else
		{
			//TRY_BEGIN_EXCEP
			ProcessResult(pResponseInfo);
			//TRY_END_EXCEP("CDBManager::ParseResponse() SyncMode");
			if (pResponseInfo->pResSet)
			{
				m_DBSys.ReclaimRecSet(pResponseInfo->pResSet);
				pResponseInfo->pResSet = NULL;
			}
			ReclaimRespInfo(pResponseInfo);
		}
	}

	//解析db返回数据,由事件触发,在主线程中调用
	void CMYSQLManagerBase::OnDataReceive( /*CMSDbManagerBase<SIZE>* pThis*/)
	{
		//LOG_DECLARE_BEGIN;
		//LOG("OnDataReceive Begin");
		MYSQLRESPONSEINFO* pResponseInfo = NULL;
		for (;;)
		{
			m_qResponse.Lock();
			if (m_qResponse.empty())
			{
#ifdef COREDEBUG
				LOG("OnDataReceive : m_qResponse.empty()");
#endif
				m_qResponse.UnLock();
				break;
			}
			pResponseInfo = (m_qResponse.pop_front());
#ifdef COREDEBUG
			LOG("OnDataReceive = %d", m_qResponse.size());
#endif
			m_qResponse.UnLock();
			//处理
			//TRY_BEGIN_EXCEP
			ProcessResult(pResponseInfo);
			//TRY_END_EXCEP("CDbManager::OnDataReceive()");
			if (pResponseInfo->pResSet)
			{
				m_DBSys.ReclaimRecSet(pResponseInfo->pResSet);
				pResponseInfo->pResSet = NULL;
			}
			ReclaimRespInfo(pResponseInfo);
		}
		//LOG_END(LOG_HANDLE_TIME, "CDbManager::OnDataReceive()");
		//LOG("OnDataReceive End");
	}

	void CMYSQLManagerBase::ProcessResult(MYSQLRESPONSEINFO* pResponseInfo)
	{
		CMYSQLQueryInfo* pQueryInfo = (CMYSQLQueryInfo*) (pResponseInfo->nKey);

		if (pQueryInfo)
		{
			if (m_mapParserFuncs.find(pQueryInfo->m_nType) != m_mapParserFuncs.end() && m_mapParserFuncs[pQueryInfo->m_nType])
			{
				m_mapParserFuncs[pQueryInfo->m_nType](pQueryInfo, pResponseInfo);
			}
			else if (pQueryInfo->m_nType != 0)
			{
				LOG("CMYSQLManagerBase::ProcessResult Invalid QueryType=%d, SP=%s Error!", pQueryInfo->m_nType, pQueryInfo->m_strSPName);
			}

			ReclaimQueryInfo(pQueryInfo);
		}
	}

	size_t CMYSQLManagerBase::GetReqInQueue()
	{
		return m_DBSys.GetAllCmdInQueue();
	}

	UINT CMYSQLManagerBase::EscapeString(char* out, const char* src, size_t len)
	{
		return m_DBSys.EscapeString(out, src, len);
	}

	MYSQLLIB::CMYSQLRecordSet* CMYSQLManagerBase::ApplyRecSet()
	{
		return m_DBSys.ApplyRecSet();
	}

	void CMYSQLManagerBase::ReclaimRecSet(MYSQLLIB::CMYSQLRecordSet* pRecSet)
	{
		m_DBSys.ReclaimRecSet(pRecSet);
	}

	MYSQLRESPONSEINFO* CMYSQLManagerBase::ApplyRespInfo()
	{
		MYSQLRESPONSEINFO* pRespInfo = m_ResponsePool.ApplyObj();

		return pRespInfo;
	}
	void CMYSQLManagerBase::ReclaimRespInfo(MYSQLRESPONSEINFO* pRespInfo)
	{
		m_ResponsePool.ReclaimObj(pRespInfo);
	}

	CMYSQLQueryInfo* CMYSQLManagerBase::ApplyQueryInfo()
	{
		CMYSQLQueryInfo* pQueryInfo = m_QueryPool.ApplyObj();

		if (pQueryInfo == NULL)
			LOG("DB=0x%08x ApplyQueryInfo Error=%d", this, GetLastError());

		return pQueryInfo;
	}
	void CMYSQLManagerBase::ReclaimQueryInfo(CMYSQLQueryInfo* pQueryInfo)
	{
		m_QueryPool.ReclaimObj(pQueryInfo);
	}

	void CMYSQLManagerBase::WaitForAllDBReqIdle()
	{	//阻塞等待所有的数据库线程完成操作，系统关闭时使用该方法
		while (m_DBSys.IsAllDBClientIdle() == FALSE)
		{
			//		LOG("MSDbManagerBase Wait For All Client Idle...");
			Sleep(100);
		}
		LOG("MSDbManagerBase WaitForAllDBReqIdle OK!");
	}

	void CMYSQLManagerBase::WaitForAllDBRespIdle()
	{
		int nRespLeft = 1;
		while (nRespLeft > 0)
		{
			m_qResponse.Lock();
			nRespLeft = m_qResponse.size();
			m_qResponse.UnLock();
			if (nRespLeft > 0)
				OnDataReceive();
		}
		LOG("MSDbManagerBase WaitForAllDBRespIdle OK!");
	}

	void CMYSQLManagerBase::Ping(UINT nHash)
	{
		CMYSQLClient* pClient = m_DBSys.GetClientHash(nHash);

		if (pClient)
		{
			pClient->Ping();
		}
	}
}