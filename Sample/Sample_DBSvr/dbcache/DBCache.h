#pragma once

struct sSQLCache : public YTSvrLib::CRecycle
{
	sSQLCache()
	{
		m_nIndex = 0;
		m_nKey = 0;
		m_strSQL.clear();
		m_strProc.clear();
	}

	virtual void Init()
	{
		m_nIndex = 0;
		m_nKey = 0;
		m_strSQL.clear();
		m_strProc.clear();
	}

	int m_nIndex;
	int m_nKey;
	string m_strSQL;
	string m_strProc;

	bool operator()(sSQLCache*& pLeft, sSQLCache*& pRight)
	{
		return (pLeft->m_nIndex < pRight->m_nIndex);
	}
};
/*
Every SQL Command Will be cached here.
*/
class CDBCache : public YTSvrLib::CSingle<CDBCache>
{
public:
	CDBCache() : m_poolSQLCache("sSQLCache")
	{
		m_nCurIndex = 0;
		m_mapSQLCache.clear();
	}

	virtual ~CDBCache()
	{

	}

	void AddSQLCache(LPCSTR lpszSQL,LPCSTR lpszProc,SQLKEY nKey1,SQLKEY nKey2,SQLKEY nKey3);
	
	void RefreshSQLCache();
private:
	int m_nCurIndex;
	std::unordered_map<string, sSQLCache*> m_mapSQLCache;
	YTSvrLib::CPool<sSQLCache, 256> m_poolSQLCache;

	sSQLCache* AllocateSQLCache();
	void ReleaseSQLCache(sSQLCache* pObj);
};