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
#ifndef _MYSQLMANAGERBASE_H_
#define _MYSQLMANAGERBASE_H_


#include "../Service/Utility.h"
#include <string>
#include "../Global/GlobalServer.h"
#include "MYSQLDBSys.h"

typedef mysqlpp::DateTime CSQLDateTime;

#define MYSQL_RESP_OUTPUTPARAM_MAX	4

#define MYSQL_RESP_USERPARAM_MAX	6

namespace YTSvrLib
{

	struct YTSVRLIB_EXPORT MYSQLRESPONSEINFO : public CRecycle
	{
		UINT				nErrorCode;
		ULONG				nAffectRows;
		void*				nKey;
		MYSQLLIB::CMYSQLRecordSet*    pResSet;

		virtual void Init()
		{
			nErrorCode = 0;
			nAffectRows = 0;
			nKey = NULL;
			pResSet = 0;
		}
	};
	typedef CWQueue<MYSQLRESPONSEINFO*> QueueMYSQLResp;


	class YTSVRLIB_EXPORT CMYSQLQueryInfo : public CRecycle
	{
	public:

		long		    m_nType;//请求类型
		char            m_strSPName[64];
		ULONGLONG		m_tTime;

		LONGLONG		m_ayParam[MYSQL_RESP_USERPARAM_MAX];

		CMYSQLQueryInfo()
		{
			Init();
		}
		virtual void Init()
		{
			m_nType = 0;
#ifdef LIB_WINDOWS
			m_tTime = GetTickCount64();
#else
			m_tTime = (ULONGLONG) time(NULL);
#endif // DEBUG
			memset(m_strSPName, 0, sizeof(m_strSPName));
			ZeroMemory(m_ayParam, sizeof(m_ayParam));
		}
	};

	typedef void(*MYSQLPARSERESPONSEFUNC)(CMYSQLQueryInfo*, MYSQLRESPONSEINFO*);

	class YTSVRLIB_EXPORT CMYSQLManagerBase
	{
	public:

		CMYSQLManagerBase() : m_QueryPool("CMYSQLQueryInfo"), m_ResponsePool("MYSQLRESPONSEINFO")
		{
			m_mapParserFuncs.clear();
			m_mapSQLName.clear();
		}

		void ParseResponse(UINT nErrorCode, ULONG nAffectRows, void* dwKey, MYSQLLIB::CMYSQLRecordSet* t_ResSet, BOOL bAsync);//解析db返回数据
		void OnDataReceive( /*CMSDbManagerBase* pThis*/);//解析db返回数据,由事件触发,在主线程中调用
		void CheckQuery();
		void ProcessResult(MYSQLRESPONSEINFO* pResponseInfo);

#define MSDBSQL_NAME_MAXLEN	128
		void Register(INT nType, MYSQLPARSERESPONSEFUNC pFunc, LPCSTR pszSQLName)
		{
			m_mapParserFuncs[nType] = pFunc;
			m_mapSQLName[nType] = pszSQLName;
		}
		virtual void SetEvent() = 0;

		MYSQLRESPONSEINFO* ApplyRespInfo();
		void ReclaimRespInfo(MYSQLRESPONSEINFO* pRespInfo);
		CMYSQLQueryInfo* ApplyQueryInfo();
		void ReclaimQueryInfo(CMYSQLQueryInfo* pQueryInfo);
		void	WaitForAllDBReqIdle();
		void	WaitForAllDBRespIdle();
		size_t	GetReqInQueue();

		MYSQLLIB::CMYSQLRecordSet* ApplyRecSet();
		void ReclaimRecSet(MYSQLLIB::CMYSQLRecordSet* pRecSet);
		void Ping(UINT nHash);

		UINT EscapeString(char* out, const char* src, size_t len);
	protected:
		std::vector<MYSQLCONNECT_INFO>		m_DBSeverList;//数据库连接参数列表
		CMYSQLDBSystem						m_DBSys;
		CPool<CMYSQLQueryInfo, 256>			m_QueryPool;
		std::unordered_map<INT, MYSQLPARSERESPONSEFUNC> m_mapParserFuncs;//处理函数
		std::unordered_map<INT, std::string>			m_mapSQLName;
		CPool<MYSQLRESPONSEINFO, 256>		m_ResponsePool;
		QueueMYSQLResp						m_qResponse;
	};
}

#endif