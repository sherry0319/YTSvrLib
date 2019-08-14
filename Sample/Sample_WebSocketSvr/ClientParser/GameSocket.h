#pragma once
#include  <algorithm>
#include "Socket/WebSocket/YTWSConnector.h"
#include "../Config/Config.h"

class GameSocket : public YTSvrLib::IWSCONNECTOR, public YTSvrLib::CRecycle
{
public:
	GameSocket()
	{
		m_tExpired = 0;
		m_nClientID = 0;
		m_mapMessageRecved.clear();
		m_sendBuf.SetQueueLenMax(1024);
		ZeroMemory(m_szIP, sizeof(m_szIP));
		m_nPort = 0;
	}

	virtual void Init();
public:
	void SetClientID(int nID)
	{
		m_nClientID = nID;
	}
	int	GetClientID()
	{
		return m_nClientID;
	}

	void InitData();

	void OnRecvNewMsg(UINT nMsgSeqno, int nMsgType);
	void OnSendMsg(UINT nMsgSeqno, int nMsgType);

	void OnClosed();
	void SendBinary(const char* info, int len);
public:
	__time32_t GetExpired() const
	{
		return m_tExpired;
	}

	void SetExpired(__time32_t t)
	{
		m_tExpired = t;
	}
public:
	void SetAddrIP(const char* pszIP)
	{
		strncpy_s(m_szIP, pszIP, 31);
	}
	const char* GetAddrIp()
	{
		return m_szIP;
	}

	int GetAddrPort()
	{
		return m_nPort;
	}
private:
	int m_nClientID;
	char m_szIP[32];
	int m_nPort;
	MessageRecord m_mapMessageRecved;
	BOOL m_bClientClosed;
	__time32_t m_tExpired;
};