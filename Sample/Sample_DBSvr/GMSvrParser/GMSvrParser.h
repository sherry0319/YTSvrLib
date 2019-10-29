#pragma once
#include "GMSvrSocket.h"
#include "Socket/TCPSocket/TCPSocket.h"
#include "Socket/MessageMapping.h"

class CGMSvrParser : public YTSvrLib::ITCPSERVER,public YTSvrLib::ITCPMSGHANDLER, public YTSvrLib::CSingle<CGMSvrParser>
{
#undef GMSVRPARSER_PROC_TABLE_CLIENT
#define GMSVRPARSER_PROC_TABLE_CLIENT(proto, proc) static int proc( YTSvrLib::ITCPBASE* pSocket, const char* pBuf, int nLen );
#include "GMSvrParseTable.h"

public:
	CGMSvrParser(void);
	virtual ~CGMSvrParser(void);

public:
	virtual void ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen) override
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

	virtual void ProcessEvent(YTSvrLib::EM_MESSAGE_TYPE emType, YTSvrLib::ITCPBASE* pConn) override;// 处理网络事件

	void CloseAllClients();

	virtual void SetEvent();

	static void OnMsgRecv()
	{
		GetInstance()->MessageConsumer();
	}

	virtual YTSvrLib::ITCPCONNECTOR* AllocateConnector(std::string strDstIP)
	{
		if (IsValidIP(strDstIP.c_str()))
		{
			return m_PoolPlayer.ApplyObj();
		}
		return NULL;
	}

	virtual void ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pConnect)
	{
		CGMSvrSocket* pGMSvrSocket = dynamic_cast<CGMSvrSocket*>(pConnect);
		if (pGMSvrSocket)
		{
			m_PoolPlayer.ReclaimObj(pGMSvrSocket);
		}
	}

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

