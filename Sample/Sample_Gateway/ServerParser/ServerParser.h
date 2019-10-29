#pragma once
#include "ServerSocket.h"
#include "Socket/TCPSocket/TCPSocket.h"

class CServerParser : public YTSvrLib::ITCPCLIENTMGR, public YTSvrLib::CSingle<CServerParser>
{
public:
	CServerParser(void);
	virtual ~CServerParser(void);

	virtual void ProcessMessage(YTSvrLib::ITCPBASE* pConn, const char* data, int len) override;

	virtual void ProcessEvent(YTSvrLib::EM_MESSAGE_TYPE emType, YTSvrLib::ITCPBASE* pConn) override;

	virtual void SetEvent();

	static void OnMsgRecv()
	{
		GetInstance()->MessageConsumer();
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
