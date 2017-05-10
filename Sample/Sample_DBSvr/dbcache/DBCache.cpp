#include "stdafx.h"
#include "DBCache.h"

sSQLCache* CDBCache::AllocateSQLCache()
{
	return m_poolSQLCache.ApplyObj();
}

void CDBCache::ReleaseSQLCache(sSQLCache* pObj)
{
	m_poolSQLCache.ReclaimObj(pObj);
}

void CDBCache::AddSQLCache(LPCSTR lpszSQL, LPCSTR lpszProc, SQLKEY nKey1, SQLKEY nKey2, SQLKEY nKey3)
{
	char szKey[1024] = { 0 };
	_snprintf_s(szKey,1023, "%s|" SQLKEYFORMAT "|" SQLKEYFORMAT "|" SQLKEYFORMAT ,lpszProc,nKey1,nKey2,nKey3);

	sSQLCache* pSQLCache = NULL;

	auto it = m_mapSQLCache.find(szKey);
	if (it != m_mapSQLCache.end())
	{
		pSQLCache = it->second;
	}

	if (pSQLCache == NULL)
	{
		pSQLCache = AllocateSQLCache();
		if (pSQLCache == NULL)
		{
			LOGERROR("Allocate sSQLCache Failed");
			return;
		}
	}

	pSQLCache->m_strProc = lpszProc;
	pSQLCache->m_strSQL = lpszSQL;
	pSQLCache->m_nKey = nKey1;
	pSQLCache->m_nIndex = (++m_nCurIndex);

	m_mapSQLCache[szKey] = pSQLCache;

	if (m_nCurIndex >= CConfig::GetInstance()->m_nSQLCacheCleanLine)
	{
		RefreshSQLCache();
	}
}

void CDBCache::RefreshSQLCache()
{
	std::list<sSQLCache*> listSQLCache;

	for (auto& pairs : m_mapSQLCache)
	{
		if (pairs.second != NULL)
		{
			listSQLCache.push_back(pairs.second);
		}
	}

	m_mapSQLCache.clear();

	listSQLCache.sort(sSQLCache());

	LOG("DBSvr RefreshSQLCache : [%d]",listSQLCache.size());

	for (auto& cache : listSQLCache)
	{
		sSQLCache* pSQLCache = cache;
		if (pSQLCache)
		{
			CDBManager::GetInstance()->OnReqExcuteSQL(pSQLCache->m_strSQL.c_str(), pSQLCache->m_strProc.c_str(), (DWORD)pSQLCache->m_nKey);

			ReleaseSQLCache(pSQLCache);
		}
	}

	m_nCurIndex = 0;
}