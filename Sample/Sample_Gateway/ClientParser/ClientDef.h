#pragma once
#pragma pack(push,1)

#define TCPFLAG_SIGN_CLIENTMSG     0xFFFFEEEE

typedef int			USERID;
typedef int			NUMBER;
typedef char		STRING;
typedef int			TIME;

enum EM_MSG_TYPE
{

};

#define RESP(_Msg) (_Msg+0x1000)

struct sClientMsg_Head
{
	sClientMsg_Head(UINT nMsgType) : m_nTCPFlag(TCPFLAG_SIGN_CLIENTMSG), m_nMsgType(nMsgType), m_nMsgLenTotal(sizeof(sClientMsg_Head)),m_nMsgSeqNo(0) {
		m_nZipEncrypFlag = 0;
		m_nZipSrcLen = 0;
	};
	void InitHead( UINT nMsgSeqno,UINT nMsgType, UINT nTotalLen ){
		m_nTCPFlag = TCPFLAG_SIGN_CLIENTMSG;
		m_nMsgType = nMsgType;
		m_nMsgLenTotal = nTotalLen;
		m_nMsgSeqNo = nMsgSeqno;
		m_nZipEncrypFlag = 0;
		m_nZipSrcLen = 0;
	}
	UINT m_nTCPFlag;	//固定标记（1 UINT）0xFFFFEEEE
	UINT m_nMsgType;	//消息类型（1 UINT）0x0001~0x0FFF
	UINT m_nMsgSeqNo;		//消息编号（1 UINT）递增
	UINT m_nMsgLenTotal;	//消息总长度（1 UINT）包括消息头长度
#define _CLIENTMSG_FLAG_ENCRYPT			0x1
#define	_CLIENTMSG_FLAG_ZIPPED			0x2
	UINT m_nZipEncrypFlag;	//消息体压缩/加密标记（UINT） 0x1=压缩|0x2=加密
	UINT m_nZipSrcLen;	//压缩前消息体原长度（UINT）
};
#define MAX_DATA_TO_ZIP		(8192<<4)

//客户端请求消息头结构定义：
struct sClientMsg_ReqHead : public sClientMsg_Head
{
	sClientMsg_ReqHead( UINT nMsgType) : sClientMsg_Head(nMsgType)
	{
		m_nUserID = 0;
		m_nMsgLenTotal = sizeof(sClientMsg_ReqHead);
	}
	void InitHead(UINT nMsgSeqno, UINT nMsgType, UINT nTotalLen)
	{
		sClientMsg_Head::InitHead( nMsgSeqno,nMsgType, nTotalLen );
		m_nUserID = 0;
	}
	BOOL CheckData( int nLen ) {
		if( nLen < sizeof(sClientMsg_ReqHead) )
			return FALSE;
		return TRUE;
	}

	USERID m_nUserID;	//玩家编号
};

//服务器响应消息头结构定义：
struct sClientMsg_RespHead : public sClientMsg_Head
{
	sClientMsg_RespHead( UINT nMsgType) : sClientMsg_Head(nMsgType)
	{
		m_nRespRet = 0;
		m_nMsgLenTotal = sizeof(sClientMsg_RespHead);
	}
	void InitHead(UINT nMsgSeqno, UINT nMsgType, UINT nTotalLen)
	{
		sClientMsg_Head::InitHead( nMsgSeqno,nMsgType, nTotalLen );
		m_nRespRet = 0;
	}
	NUMBER m_nRespRet;	//响应结果（1 UINT）
};

//服务器通知消息头结构定义
struct sClientNotify_Head : public sClientMsg_Head
{
	sClientNotify_Head( UINT nMsgType) : sClientMsg_Head(nMsgType)
	{
		m_nMsgLenTotal = sizeof(sClientNotify_Head);
		m_nRet = 0;
	}

	NUMBER m_nRet;
};

#pragma pack(pop)
