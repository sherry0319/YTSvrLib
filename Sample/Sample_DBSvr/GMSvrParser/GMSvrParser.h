#pragma once
#include "GMSvrSocket.h"
#include "Socket/YTSocketServer.h"

class CGMSvrParser : public YTSvrLib::ITCPSERVER, public YTSvrLib::CPkgParserBase, public YTSvrLib::CSingle<CGMSvrParser>
{
#undef GMSVRPARSER_PROC_TABLE_CLIENT
#define GMSVRPARSER_PROC_TABLE_CLIENT(proto, proc) static int proc( YTSvrLib::ITCPBASE* pSocket, const char* pBuf, int nLen );
#include "GMSvrParseTable.h"

public:
	CGMSvrParser(void);
	virtual ~CGMSvrParser(void);

public:
	virtual void ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen)
	{
		if (nLen > 1)//Normal Package
		{
			LPSDBMsgHead pMsgHead = (LPSDBMsgHead)pBuf;
			if (pMsgHead->m_wMsgType > 0 && m_CltMsgProcs.find(pMsgHead->m_wMsgType) != m_CltMsgProcs.end() && m_CltMsgProcs[pMsgHead->m_wMsgType])
			{
				m_CltMsgProcs[pMsgHead->m_wMsgType]( pSocket, pBuf, nLen );
			}
			else if( pMsgHead->m_wMsgType != 0 )
				LOG("GMSvrSocket=%d Invalid Msg=0x%04x Len=%d Error!", pSocket->GetSocket(),pMsgHead->m_wMsgType, nLen );
		}
	}

	void CloseAllClients();

	virtual void SetEvent();
	virtual void SetDisconnectEvent();

	static void OnMsgRecv()
	{
		GetInstance()->OnMessageRecv();
	}

	static void OnDisconnectMsgRecv()
	{
		GetInstance()->OnDisconnectMessage();
	}

	virtual YTSvrLib::ITCPCONNECTOR* AllocateConnector()
	{
		return m_PoolPlayer.ApplyObj();
	}

	virtual void ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pConnect)
	{
		CGMSvrSocket* pGMSvrSocket = dynamic_cast<CGMSvrSocket*>(pConnect);
		if (pGMSvrSocket)
		{
			m_PoolPlayer.ReclaimObj(pGMSvrSocket);
		}
	}

	virtual void ProcessAcceptedMsg(YTSvrLib::ITCPBASE* pSocket);

	virtual void ProcessDisconnectMsg(YTSvrLib::ITCPBASE* pSocket);

	void OnDisconnected(CGMSvrSocket* pGMSock);

	virtual void OnServerClose()
	{
		CloseAllClients();
	}

	BOOL IsValidIP( const char* pszRemoteIP );

private:
	std::set<CGMSvrSocket*> m_listClients;
	YTSvrLib::CPool<CGMSvrSocket,4>  m_PoolPlayer;
};

