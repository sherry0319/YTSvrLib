#include "stdafx.h"
#include "GWServerParser.h"

CGWSvrParser::CGWSvrParser(void) : m_poolGateway("CGMSvrSocket")
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
	}
}

void CGWSvrParser::ProcessEvent(YTSvrLib::EM_MESSAGE_TYPE emType, YTSvrLib::ITCPBASE* pConn) {
	LOG("ProcessEvent TYPE=[%d] CONN=[0x%x]",emType,pConn);
	CGWSvrSocket* pGWSvrSocket = dynamic_cast<CGWSvrSocket*>(pConn);
	switch (emType)
	{
	case YTSvrLib::MSGTYPE_DISCONNECT: {
		OnGWSvrDisconnect(pGWSvrSocket);
	}break;
	case YTSvrLib::MSGTYPE_ACCEPTED: {
		OnGWSvrConnected(pGWSvrSocket);
	}break;
	default:
		break;
	}
}

YTSvrLib::ITCPCONNECTOR* CGWSvrParser::AllocateConnector(std::string dstIP)
{
	int nRemoteAddr = inet_addr(dstIP.c_str());
	for (UINT i = 0; i < CConfig::GetInstance()->m_vctClientIPWhiteList.size(); i++)
	{
		if (CConfig::GetInstance()->m_vctClientIPWhiteList[i] == 0)
			return NULL;
		if (CConfig::GetInstance()->m_vctClientIPWhiteList[i] == nRemoteAddr)
			return NULL;
	}
	return m_poolGateway.ApplyObj();
}

void CGWSvrParser::ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pConn)
{
	CGWSvrSocket* pGWSvrSocket = dynamic_cast<CGWSvrSocket*>(pConn);
	if (pGWSvrSocket)
	{
		m_poolGateway.ReclaimObj(pGWSvrSocket);
	}
}

void CGWSvrParser::OnGWSvrConnected(CGWSvrSocket* pGWSvrSocket)
{
	LOG("CGWSvrSocket Connected : %s:%d On Socket(0x%x)", pGWSvrSocket->GetAddrIp().c_str(), pGWSvrSocket->GetAddrPort(), pGWSvrSocket);
}

void CGWSvrParser::OnGWSvrDisconnect(CGWSvrSocket* pSocket)
{
	LOG("CGWSvrSocket Disconnected : %s:%d On Socket(0x%x)", pSocket->GetAddrIp().c_str(), pSocket->GetAddrPort(), pSocket);

	pSocket->OnClosed();
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
	YTSvrLib::CServerApplication::GetInstance()->SetEvent(EAppEvent::eAppGWSvrSocketEvent);
}
