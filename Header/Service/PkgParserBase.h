/*MIT License

Copyright (c) 2016 Archer Xu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
#pragma once

#include <event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include "../stl/wqueue.h"
//////////////////////////////////////////////////////////////////////////
#include "../Socket/YTSocketBase.h"

namespace YTSvrLib
{

	typedef struct YTSVRLIB_EXPORT _PKGINFO : public CRecycle
	{
		YTSvrLib::ITCPBASE *pSocket;
		std::string sContent;
		virtual void Init()
		{
			pSocket = 0;
			sContent.clear();
		}
	}MsgPkg, *PMsgPkg;

	typedef struct YTSVRLIB_EXPORT _DISCONNECTINFO : public CRecycle
	{
		EType        eType;
		YTSvrLib::ITCPBASE* pSocket;

		virtual void Init()
		{
			pSocket = 0;
			eType = eInvalid;
		}
	}DisconnectPkg, *PDisconnectPkg;

	class YTSVRLIB_EXPORT CParserBase
	{
	protected:
		CParserBase() :m_PoolMsgPkg("MsgPkg"), m_PoolDisconnectPkg("DisconnectPkg")
		{

		}
		virtual void SetEvent() = 0;

		virtual void SetDisconnectEvent() = 0;
	public:

		virtual void Start()
		{};
		virtual void PostPkgMsg(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen);

		virtual void PostDisconnMsg(YTSvrLib::ITCPBASE* pSocket, EType eType);
	protected:
		virtual void ProcessMessage(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen) = 0;

		virtual void ProcessDisconnectMsg(YTSvrLib::ITCPBASE* pSocket) = 0;

		virtual void ProcessAcceptedMsg(YTSvrLib::ITCPBASE*)
		{};

		virtual void OnMessageRecv();

		virtual void OnDisconnectMessage();
	protected:
		virtual void AddToQueue(PMsgPkg pMsgPkg);

		virtual void AddToDisconnectQueue(PDisconnectPkg pPkg);
	protected:
		CPool<MsgPkg, 256>           m_PoolMsgPkg;
		CPool<DisconnectPkg, 128>    m_PoolDisconnectPkg;

		CWQueue<PMsgPkg>		m_qMsg;

		CWQueue<PDisconnectPkg>   m_qDisconnectMsg;
	};

	typedef void(*CLTEVENTHTTPFUNC)(evhttp_request* req);

	class YTSVRLIB_EXPORT CEventHttpParser
	{
	protected:
		CEventHttpParser()
		{

		}

		virtual void SetEvent() = 0;

		virtual void ProcessMessage(evhttp_request* req) = 0;

		virtual void PostRequest(evhttp_request* req);

		virtual void OnMessageRecv();

		virtual void AddToQueue(evhttp_request* req);

		virtual void Register(const char* path, CLTEVENTHTTPFUNC pFunc)
		{
			m_mapMsgProcs[string(path)] = pFunc;
		}
	protected:
		CWQueue<evhttp_request*> m_qMsg;
		std::unordered_map<string, CLTEVENTHTTPFUNC> m_mapMsgProcs;
	};

	typedef int(*CLTPSRFUNC)(YTSvrLib::ITCPBASE* pSocket, const char* pBuf, int nLen);

	class YTSVRLIB_EXPORT CPkgParserBase :public CParserBase
	{
	protected:
		std::unordered_map<int, CLTPSRFUNC> m_CltMsgProcs;
	protected:
		CPkgParserBase()
		{
			m_CltMsgProcs.clear();
		}
	protected:
		virtual void Register(WORD wType, CLTPSRFUNC pFunc)
		{
			m_CltMsgProcs[wType] = pFunc;
		}
	};

	typedef std::unordered_map<std::string, std::string> CMapString;
	typedef void(*CLTPSRHTTPFUNC)(YTSvrLib::ITCPBASE* pSocket, CMapString& mapParam);

	class YTSVRLIB_EXPORT CHttpParserBase :public CParserBase
	{
	protected:
		std::unordered_map<int, CLTPSRHTTPFUNC> m_CltMsgProcs;
		std::unordered_map<int, UINT> m_MsgNeedParams;
	protected:

		CHttpParserBase()
		{
			m_CltMsgProcs.clear();
			m_MsgNeedParams.clear();
		}

	protected:
		virtual void Register(WORD wType, CLTPSRHTTPFUNC pFunc, UINT nNeedParamCnt)
		{
			m_CltMsgProcs[wType] = pFunc;
			m_MsgNeedParams[wType] = nNeedParamCnt;
		}

		static int ParseHttpParam(LPCSTR lpszHttpMsg, CMapString& mapParams);
	public:
		int SendHttpErrorMsg(YTSvrLib::ITCPBASE* pSocket, UINT nRes, LPCSTR szMsgFormat, ...);

		int SendHttpMsg(YTSvrLib::ITCPBASE* pSocket, const char *pBuf);

	};
}