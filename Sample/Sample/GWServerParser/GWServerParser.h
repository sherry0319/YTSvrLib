#pragma once
#include "GWServerSocket.h"
#include "Socket/YTSocketServer.h"

class CGWSvrParser : public YTSvrLib::ITCPSERVER, public YTSvrLib::CPkgParserBase, public YTSvrLib::CSingle<CGWSvrParser>
{
public:
	CGWSvrParser(void);
	virtual ~CGWSvrParser(void);

public:
	virtual void SetEvent();
	virtual void SetDisconnectEvent();

	static void OnMsgRecv()// 消息接收完成.触发处理
	{
		GetInstance()->OnMessageRecv();
	}

	static void OnDisconnectMsgRecv()// 收到断开连接的消息.触发处理.
	{
		GetInstance()->OnDisconnectMessage();
	}

	virtual void ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen);// 处理普通的网络消息

	virtual void ProcessAcceptedMsg(YTSvrLib::ITCPBASE* pSocket);// 处理Accept连接消息

	virtual void ProcessDisconnectMsg(YTSvrLib::ITCPBASE* pSocket);// 处理断开连接消息

	virtual YTSvrLib::ITCPCONNECTOR* AllocateConnector();// 分配一个连接者

	virtual void ReleaseConnector(YTSvrLib::ITCPCONNECTOR* pConnect);// 销毁一个连接者

	virtual void OnServerClose(){}// 处理服务器销毁

	void OnGWSvrConnected(CGWSvrSocket* pGWSvrSocket);
	void OnGWSvrDisconnect( CGWSvrSocket* pSocket );

	void SendClientMsg( CGWSvrSocket* pGWSocket , LPCSTR pszMsg, int nLen );
	void SendSvrMsg( CGWSvrSocket* pGWSocket , LPCSTR pszMsg, int nMsgLen);
private:
	YTSvrLib::CPool<CGWSvrSocket,2>  m_poolGateway;
};