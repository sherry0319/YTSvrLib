#pragma once
#pragma pack(push,1)

//网关消息包头标记定义
#define TCPFLAG_GWMSG		0xFFEEDDCC
//网关消息类型定义
#define T_GWMSG_KEEPALIVE			0	//网关心跳包
#define T_GWMSG_SVRLOGIN			1	//网关登陆
#define T_GWMSG_C2S					2	//客户端消息
#define T_GWMSG_S2C					3	//服务器->客户端
#define T_GWMSG_S2S					4	//服务器->服务器
#define T_GWMSG_S2S_ERR				5	//服务器错误消息
#define T_GWMSG_AS2US				6	
#define T_GWMSG_ADMIN2USER			7
#define T_GWMSG_CLIENTLOGOUT		8
#define T_GWMSG_CLIENTBROADCAST		10	//客户端广播消息

//客户端/服务器类型定义
typedef enum {
	emAgent_Null = 0,
	emAgent_Client = 1,	//客户端
	emAgent_GateWay = 10,	//网关
	emAgent_UserSvr = 20,	//逻辑服务器1
	emAgent_UserSvr2 = 30,	//逻辑服务器2
} EM_AGENT;

struct sAgent
{
	EM_AGENT m_emType;
	UINT m_nAgentID;
};

struct sGWMsg_Head
{
	sGWMsg_Head( UINT nMsgType ) : m_nTcpFlag(TCPFLAG_GWMSG),m_nMsgType(nMsgType)
	{
		m_nTotalMsgLen = sizeof(*this);
		ZeroMemory( &m_From, sizeof(m_From) );
		ZeroMemory( &m_To, sizeof(m_To) );
		ZeroMemory( &m_RouteBy, sizeof(m_RouteBy) );
	};
	void Init( UINT nMsgType, UINT nTotalLen ) {
		m_nTcpFlag = TCPFLAG_GWMSG;
		m_nTotalMsgLen = nTotalLen;
		m_nMsgType = nMsgType;
		ZeroMemory( &m_From, sizeof(m_From) );
		ZeroMemory( &m_To, sizeof(m_To) );
		ZeroMemory( &m_RouteBy, sizeof(m_RouteBy) );
	}
	UINT	m_nTcpFlag;			//固定标识符（4Byte）：
	UINT	m_nTotalMsgLen;  //总长度，包括包头长度
	UINT	m_nMsgType;		
	
	char	m_szClientIP[32];//客户端IP地址
	sAgent m_From;	//发送者信息
	sAgent m_To;	//接收方信息
	sAgent m_RouteBy;	//中转方信息
};

struct sGWMsg_KeepAlive : public sGWMsg_Head
{
	sGWMsg_KeepAlive() : sGWMsg_Head( T_GWMSG_KEEPALIVE )
	{
		m_nTotalMsgLen = sizeof(*this);
		m_nRunParam = 0;
	}
	UINT m_nRunParam;
};

struct sGWMsg_Login : public sGWMsg_Head
{
	sGWMsg_Login() : sGWMsg_Head( T_GWMSG_SVRLOGIN )
	{
		m_nTotalMsgLen = sizeof(*this);
		m_nGameID = 0;
	}
	UINT	m_nGameID;		//游戏ID
	UINT	m_nPublicSvrID;	//服务器组ID
};

struct sGWMsg_ClientDisconnect : public sGWMsg_Head
{
	sGWMsg_ClientDisconnect() : sGWMsg_Head( T_GWMSG_CLIENTLOGOUT )
	{
		m_nTotalMsgLen = sizeof(*this);
	}

	UINT m_nSvrID;
	UINT m_nGWID;
	UINT m_nClientID;
};

#pragma pack(pop)
