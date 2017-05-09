#include "stdafx.h"
#include "GWServerParser.h"

extern YTSvrLib::CServerApplication gApp;
//////////////////////////////////////////////////////////////////////////

CGWSvrParser::CGWSvrParser(void) : YTSvrLib::CPkgParserBase(), m_poolGateway("CGMSvrSocket")
{

}

CGWSvrParser::~CGWSvrParser(void)
{

}

void CGWSvrParser::ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen)
{
	if (nLen > 1)//正常数据包
	{
		LOG("Recv message : from=[%s:%d] lenth=[%d]", pSocket->GetAddrIp(), pSocket->GetAddrPort(), nLen);

		CGWSvrSocket* pGWSvrSocket = dynamic_cast<CGWSvrSocket*>(pSocket);
		if (pGWSvrSocket)
		{
			CDBManager::GetInstance()->SendSomeQuery(pGWSvrSocket);
		}
	}
}

void CGWSvrParser::ProcessAcceptedMsg(YTSvrLib::ITCPBASE* pSocket)
{
	CGWSvrSocket* pGWSvrSocket = dynamic_cast<CGWSvrSocket*>(pSocket);
	if (pGWSvrSocket)
	{
		OnGWSvrConnected(pGWSvrSocket);
	}
}

void CGWSvrParser::ProcessDisconnectMsg(YTSvrLib::ITCPBASE* pSocket)
{
	CGWSvrSocket* pGWSvrSocket = dynamic_cast<CGWSvrSocket*>(pSocket);
	if (pGWSvrSocket)
	{
		OnGWSvrDisconnect(pGWSvrSocket);
	}
}

YTSvrLib::ITCPCONNECTOR* CGWSvrParser::AllocateConnector()
{
	return m_poolGateway.ApplyObj();
}

void CGWSvrParser::ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pConnect)
{
	CGWSvrSocket* pGWSvrSocket = dynamic_cast<CGWSvrSocket*>(pConnect);
	if (pGWSvrSocket)
	{
		m_poolGateway.ReclaimObj(pGWSvrSocket);
	}
}

void CGWSvrParser::OnGWSvrConnected(CGWSvrSocket* pGWSvrSocket)
{
	LOG("CGWSvrSocket Connected : %s:%d On Socket(%d)", pGWSvrSocket->GetAddrIp(), pGWSvrSocket->GetAddrPort(), pGWSvrSocket->GetSocket());
}

void CGWSvrParser::OnGWSvrDisconnect(CGWSvrSocket* pSocket)
{
	LOG("CGWSvrSocket Disconnected : %s:%d On Socket(%d)", pSocket->GetAddrIp(), pSocket->GetAddrPort(), pSocket->GetSocket());
	pSocket->SafeClose();
}

void CGWSvrParser::SendClientMsg(CGWSvrSocket* pGWSocket, LPCSTR pszMsg, int nLen)
{
	if (pGWSocket == NULL)
		return;

	pGWSocket->Send(pszMsg, nLen);
}

void CGWSvrParser::SendSvrMsg(CGWSvrSocket* pGWSocket, LPCSTR pszMsg, int nMsgLen)
{
	if (pGWSocket == NULL)
		return;

	pGWSocket->Send(pszMsg, nMsgLen);
}

void CGWSvrParser::SetEvent()
{
	gApp.SetEvent( EAppEvent::eAppGWSvrSocketEvent );
}

void CGWSvrParser::SetDisconnectEvent()
{
	gApp.SetEvent( EAppEvent::eAppGWSvrSocketDisconnectEvent );
}