#pragma once
#include "GWServerSocket.h"
#include "Socket/TCPSocket/TCPSocket.h"

class CGWSvrParser : public YTSvrLib::ITCPSERVER, public YTSvrLib::CSingle<CGWSvrParser>
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

	virtual void ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen) override;// 处理网络消息

	virtual void ProcessEvent(YTSvrLib::EM_MESSAGE_TYPE emType, YTSvrLib::ITCPBASE* pConn) override;// 处理网络事件

	virtual YTSvrLib::ITCPCONNECTOR* AllocateConnector(std::string);// 分配一个连接者

	virtual void ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pConnect);// 销毁一个连接者

	virtual void OnServerClose(){}// 处理服务器销毁

	void OnGWSvrConnected(CGWSvrSocket* pGWSvrSocket);
	void OnGWSvrDisconnect( CGWSvrSocket* pSocket );

	void SendClientMsg( CGWSvrSocket* pGWSocket , LPCSTR pszMsg, int nLen );
	void SendSvrMsg( CGWSvrSocket* pGWSocket , LPCSTR pszMsg, int nMsgLen);
private:
	YTSvrLib::CPool<CGWSvrSocket,2>  m_poolGateway;
};