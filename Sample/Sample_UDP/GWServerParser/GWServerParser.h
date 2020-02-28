#pragma once
#include "GWServerSocket.h"
#include "Socket/RUDP/UDPSocket.h"

class CGWSvrParser : public YTSvrLib::IUDPSERVER, public YTSvrLib::CSingle<CGWSvrParser>
{
public:
	CGWSvrParser(void);
	virtual ~CGWSvrParser(void);

public:
	virtual void SetEvent();

	static void OnMsgRecv()// 消息接收完成.触发处理
	{
		GetInstance()->MessageConsumer();
	}

	virtual void ProcessMessage(YTSvrLib::IUDPBASE* pSocket, const char *pBuf, int nLen) override;// 处理普通的网络消息

	virtual void ProcessEvent(YTSvrLib::EM_MESSAGE_TYPE emType, YTSvrLib::IUDPBASE* pConn) override;

	virtual YTSvrLib::IUDPCONNECTOR* AllocateConnector(std::string dstIP) override;// 分配一个连接者

	virtual void ReleaseConnector(YTSvrLib::IUDPCONNECTOR* pConn);// 销毁一个连接者

	void OnGWSvrConnected(CGWSvrSocket* pGWSvrSocket);
	void OnGWSvrDisconnect( CGWSvrSocket* pSocket );

	void SendClientMsg( CGWSvrSocket* pGWSocket , LPCSTR pszMsg, int nLen );
	void SendSvrMsg( CGWSvrSocket* pGWSocket , LPCSTR pszMsg, int nMsgLen);
private:
	YTSvrLib::CPool<CGWSvrSocket,2>  m_poolGateway;
};