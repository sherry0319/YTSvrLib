#pragma once
#ifndef __WS_PKG_PARSER_H_
#define __WS_PKG_PARSER_H_

namespace YTSvrLib {

	class IWSCONNECTOR;
	struct YTSVRLIB_EXPORT WSMSG : public CRecycle
	{
		IWSCONNECTOR* pConn;
		std::string msg;

		virtual void Init()
		{
			msg.clear();
		}
	};

	struct YTSVRLIB_EXPORT WSEVENT : public CRecycle
	{
		IWSCONNECTOR* pConn;
		WSEType eType;
		virtual void Init()
		{
			eType = WSEType_Invalid;
		}
	};

	class YTSVRLIB_EXPORT CWSParserBase
	{
	protected:
		CWSParserBase();

		virtual void SetEvent() = 0;

		virtual void SetDisconnectEvent() = 0;
	public:
		virtual void onWSMsgRecv();

		virtual void onWSEventRecv();
	public:
		virtual void ProcessMessage(IWSCONNECTOR* pConn, const char* msg, int len) = 0;

		virtual void ProcessDisconnectMsg(IWSCONNECTOR* pConn) = 0;

		virtual void ProcessAcceptedMsg(IWSCONNECTOR* pConn) = 0;
	public:
		virtual void postWSMsg(IWSCONNECTOR* pConn, const char* msg, int len);

		virtual void postWSEvent(IWSCONNECTOR* pConn, WSEType type);

		virtual void addWSMsg(WSMSG* pPkg);

		virtual void addWSEvent(WSEVENT* pPkg);
	protected:
		CPool<WSMSG, 128>           m_PoolMsgPkg;
		CPool<WSEVENT, 128>    m_PoolDisconnectPkg;

		CWQueue<WSMSG*>		m_qMsg;
		CWQueue<WSEVENT*>   m_qDisconnectMsg;
	};
}

#endif