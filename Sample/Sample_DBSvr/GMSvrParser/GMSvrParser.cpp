#include "stdafx.h"
#include "GMSvrParser.h"

CGMSvrParser::CGMSvrParser(void) : YTSvrLib::CPkgParserBase(), m_PoolPlayer("CGMSvrSocket")
{
#undef GMSVRPARSER_PROC_TABLE_CLIENT
#define GMSVRPARSER_PROC_TABLE_CLIENT(proto, proc) Register(proto, proc);
#include "GMSvrParseTable.h"

}

CGMSvrParser::~CGMSvrParser(void)
{
}


extern time_t gTime;


void CGMSvrParser::SetEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent( EAppEvent::eAppGMSvrSocketEvent );
}

void CGMSvrParser::SetDisconnectEvent()
{
	YTSvrLib::CServerApplication::GetInstance()->SetEvent( EAppEvent::eAppGMSvrSocketDisconnectEvent );
}

BOOL CGMSvrParser::IsValidIP( const char* pszRemoteIP )
{
	int nRemoteAddr = inet_addr( pszRemoteIP );
	for( UINT i=0; i< CConfig::GetInstance()->m_vctGMSvrIPWhiteList.size(); i++ )
	{
		if( CConfig::GetInstance()->m_vctGMSvrIPWhiteList[i] == 0 )
			return TRUE;
		if( CConfig::GetInstance()->m_vctGMSvrIPWhiteList[i] == nRemoteAddr )
			return TRUE;
	}
	LOG("GMSvrParser_IsValidIP Invalid RemoteIP=%s Error!", pszRemoteIP );
	return FALSE;
}

void CGMSvrParser::ProcessAcceptedMsg(YTSvrLib::ITCPBASE* pSocket)
{
	CGMSvrSocket* pGMSvrSocket = dynamic_cast<CGMSvrSocket*>(pSocket);
	if (pGMSvrSocket)
	{
		m_listClients.insert(pGMSvrSocket);
	}
}

void CGMSvrParser::ProcessDisconnectMsg(YTSvrLib::ITCPBASE* pSocket)
{
	CDBCache::GetInstance()->RefreshSQLCache();
	CGMSvrSocket* pGMSvrSocket = dynamic_cast<CGMSvrSocket*>(pSocket);
	if (pGMSvrSocket)
	{
		pGMSvrSocket->SafeClose();
		m_listClients.erase(pGMSvrSocket);
	}
}

int CGMSvrParser::OnReqSvrLogin(YTSvrLib::ITCPBASE* pSocket, const char* pBuf, int nLen)
{
	LPSDBMsgHead pReq = (LPSDBMsgHead)pBuf;
	LOG("GMSvrParser_OnReqSvrLogin GMSvrSocket=%d IP=%s", pSocket->GetSocket(), pSocket->GetAddrIp() );

	CDBCache::GetInstance()->RefreshSQLCache();

	return nLen;
}

int CGMSvrParser::OnReqExecSQL(YTSvrLib::ITCPBASE* pSocket, const char* pBuf, int nLen)
{
	LPSDBMsgExecSql pReq = (LPSDBMsgExecSql)pBuf;
	LOG("Socket=%d Recv Execute SQL : %s",pSocket->GetSocket(),pReq->m_szProcName);

	if (CConfig::GetInstance()->m_bIsSQLCache)
	{
		if (pReq->m_bForceWrite)
		{
			CDBManager::GetInstance()->OnReqExcuteSQL(pReq->GetSQL(), pReq->m_szProcName, pReq->m_nKey);
		}
		else
		{
			CDBCache::GetInstance()->AddSQLCache(pReq->GetSQL(), pReq->m_szProcName, pReq->m_nKey, pReq->m_nKey2, pReq->m_nKey3);
		}
	}
	else
	{
		CDBManager::GetInstance()->OnReqExcuteSQL(pReq->GetSQL(), pReq->m_szProcName, pReq->m_nKey);
	}

	return nLen;
}
int CGMSvrParser::OnReqQuery(YTSvrLib::ITCPBASE* pSocket, const char* pBuf, int nLen)
{
	sDBMsgQuery sResp;
	sResp.m_dwQueueLen = CDBManager::GetInstance()->GetExcuteSQLQueueLen();
	((CGMSvrSocket*)pSocket)->Send( (char*)&sResp, sizeof(sResp) );
	return nLen;
}

void CGMSvrParser::CloseAllClients()
{
	std::set<CGMSvrSocket*>::iterator it = m_listClients.begin();
	while (it != m_listClients.end())
	{
		if (*it)
		{
			(*it)->SafeClose();
		}
		++it;
	}
	m_listClients.clear();
}