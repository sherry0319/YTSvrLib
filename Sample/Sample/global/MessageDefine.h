#pragma once
#pragma pack(push,1)

//网关消息包头标记定义
#define TCPFLAG_GWMSG		0x00000001

//客户端/服务器类型定义
typedef enum
{
	emAgent_Null = 0,
	emAgent_Client = 1,	//客户端
	emAgent_GateWay = 10,	//网关
} EM_AGENT;

struct sAgent
{
	EM_AGENT m_emType;
	UINT m_nAgentID;
};

struct sGWMsg_Head
{
	sGWMsg_Head(UINT nMsgType) : m_nTcpFlag(TCPFLAG_GWMSG), m_nMsgType(nMsgType)
	{
		m_nTotalMsgLen = sizeof(*this);
		ZeroMemory(&m_From, sizeof(m_From));
		ZeroMemory(&m_To, sizeof(m_To));
		ZeroMemory(&m_RouteBy, sizeof(m_RouteBy));
	};
	void Init(UINT nMsgType, UINT nTotalLen)
	{
		m_nTcpFlag = TCPFLAG_GWMSG;
		m_nTotalMsgLen = nTotalLen;
		m_nMsgType = nMsgType;
		ZeroMemory(&m_From, sizeof(m_From));
		ZeroMemory(&m_To, sizeof(m_To));
		ZeroMemory(&m_RouteBy, sizeof(m_RouteBy));
	}
	UINT	m_nTcpFlag;			//固定标识符（4Byte）：
	UINT	m_nTotalMsgLen;  //总长度，包括包头长度
	UINT	m_nMsgType;

	char	m_szClientIP[32];//客户端IP地址
	sAgent m_From;	//发送者信息
	sAgent m_To;	//接收方信息
	sAgent m_RouteBy;	//中转方信息
};

#pragma pack(pop)
