#pragma once
#include "ServerSocket.h"
#include "Socket/YTSocketBase.h"
//#include "..\Common\CfgServerList.h"

class CServerParser : public YTSvrLib::CPkgParserBase, public YTSvrLib::CSingle<CServerParser>
{
public:
	CServerParser(void);
	virtual ~CServerParser(void);

	virtual void ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen);

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

	virtual void ReclaimObj(YTSvrLib::ITCPBASE* pSocket)
	{
		CServerSocket* pSvrSocket = dynamic_cast<CServerSocket*>(pSocket);
		if (pSvrSocket)
		{
			m_PoolSvrSocket.ReclaimObj(pSvrSocket);
		}
	}

	virtual void ProcessDisconnectMsg(YTSvrLib::ITCPBASE* pSocket)
	{
		CServerSocket* pSvrSocket = dynamic_cast<CServerSocket*>(pSocket);
		if (pSvrSocket)
		{
			pSvrSocket->OnDisconnect();
		}
	}

	void OnSvrDisconnect( CServerSocket* pSocket );
	CServerSocket* GetSvrSocket( EM_AGENT emSvrType);
	void	CheckSvrSocket();
	void	InitSvrSocket();

	void	CloseServer();

	CServerSocket* InitUserSvrSocket();
	CServerSocket* GetUserSvrSocket(){return m_pUserSocket;}

	CServerSocket* InitUserSvrSocket2();
	CServerSocket* GetUserSvrSocket2() { return m_pUserSocket2;}
private:
	YTSvrLib::CPool<CServerSocket,16>  m_PoolSvrSocket;
	
	//后台服务器
	CServerSocket* m_pUserSocket;
	CServerSocket* m_pUserSocket2;

	__time32_t m_tLastSendGSM;
	__time32_t m_tLastSendGSMUser;
};
