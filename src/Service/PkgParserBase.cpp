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
#include "stdafx.h"
#include "PkgParserBase.h"

namespace YTSvrLib
{
	void CParserBase::PostPkgMsg(YTSvrLib::ITCPBASE* pSocket, const char *pBuf, int nLen)
	{
		PMsgPkg pMsgPkg = m_PoolMsgPkg.ApplyObj();
		if (pMsgPkg)
		{
			pMsgPkg->sContent.assign(pBuf, nLen);
			pMsgPkg->pSocket = pSocket;
			CParserBase::AddToQueue(pMsgPkg);
		}
	}

	void CParserBase::PostDisconnMsg(YTSvrLib::ITCPBASE* pSocket, EType eType)
	{
		PDisconnectPkg pPkg = m_PoolDisconnectPkg.ApplyObj();
		if (pPkg)
		{
			pPkg->pSocket = pSocket;
			pPkg->eType = eType;
			CParserBase::AddToDisconnectQueue(pPkg);
		}
	}

	void CParserBase::OnMessageRecv()
	{
		PMsgPkg pMsgPkg = 0;
		//CPkgParserBase* pPkgParser = (CPkgParserBase*)this;//GetInstance();
		UINT nCount = 0;
		BOOL bNeedSetEvt = FALSE;

		for (;;)
		{
			m_qMsg.Lock();
			if (m_qMsg.empty())
			{
				m_qMsg.UnLock();
				break;
			}
			if (nCount >= 128)
			{	//避免其他事件队列堵塞
				bNeedSetEvt = TRUE;
				m_qMsg.UnLock();
				break;
			}
			pMsgPkg = m_qMsg.pop_front();
			m_qMsg.UnLock();

			ProcessMessage(pMsgPkg->pSocket, pMsgPkg->sContent.c_str(), (int) pMsgPkg->sContent.size());

			m_PoolMsgPkg.ReclaimObj(pMsgPkg);
			nCount++;
		}
		if (bNeedSetEvt)
			SetEvent();
	}

	void CParserBase::OnDisconnectMessage()
	{
		PDisconnectPkg pPkg = 0;
		//CPkgParserBase* pPkgParser = (CPkgParserBase*)this;//GetInstance();
		for (;;)
		{
			m_qDisconnectMsg.Lock();
			if (m_qDisconnectMsg.empty())
			{
				m_qDisconnectMsg.UnLock();
				break;
			}
			pPkg = m_qDisconnectMsg.pop_front();
			m_qDisconnectMsg.UnLock();
			if (pPkg->eType == eAccepted)
			{
				ProcessAcceptedMsg(pPkg->pSocket);
			}
			else
			{
#ifdef _DEBUG
				LOG("Pkg=0x%08x Socket=0x%08x ProcessDisconnectMsg=%d", pPkg, pPkg->pSocket, pPkg->eType);
#endif
				ProcessDisconnectMsg(pPkg->pSocket);
			}

			m_PoolDisconnectPkg.ReclaimObj(pPkg);
		}
	}

	void CParserBase::AddToQueue(PMsgPkg pMsgPkg)
	{
		BOOL bMustSetEvent = FALSE;
		m_qMsg.Lock();
		if (m_qMsg.empty())
		{
			bMustSetEvent = TRUE;
		}
		m_qMsg.push_back(pMsgPkg);
		m_qMsg.UnLock();
		if (bMustSetEvent)
		{
			SetEvent();//g_Application.SetEvent(ei::eiClientMsg);
		}
	}

	void CParserBase::AddToDisconnectQueue(PDisconnectPkg pPkg)
	{
		BOOL bMustSetEvent = FALSE;
		m_qDisconnectMsg.Lock();
		if (m_qDisconnectMsg.empty())
		{
			bMustSetEvent = TRUE;
		}
		m_qDisconnectMsg.push_back(pPkg);
		m_qDisconnectMsg.UnLock();
		if (bMustSetEvent)
		{
			SetDisconnectEvent();
		}
#ifdef _DEBUG
		LOG("Pkg=0x%08x Socket=0x%08x AddToDisconnectQueue", pPkg, pPkg->pSocket);
#endif
	}


	//////////////////////////////////////////////////////////////////////////

	int CHttpParserBase::ParseHttpParam(LPCSTR lpszHttpMsg, CMapString& mapParams)
	{
		string strHttpMsg(lpszHttpMsg);

		mapParams.clear();

		string strHead = strHttpMsg.substr(0, 4);
		if (strHead == "GET ")
		{
			strHttpMsg.erase(0, 4);

			size_t nPos = strHttpMsg.find('?');

			string strPage = strHttpMsg.substr(0, nPos);

			mapParams[std::move(std::string("request_page"))] = strPage;

			strHttpMsg.erase(0, nPos + 1);

			nPos = strHttpMsg.find(" HTTP");

			if (nPos != strHttpMsg.npos)
			{
				strHttpMsg.erase(nPos);
			}
		}
		else if (strHead == "POST")
		{
			return 1;
		}

		strHttpMsg.append(1, '&');
		size_t nPos = strHttpMsg.find('&');

		while (nPos != strHttpMsg.npos)
		{
			string strParam = strHttpMsg.substr(0, nPos);

			size_t nParamPos = strParam.find('=');

			string strKey = strParam.substr(0, nParamPos);

			string strValue = strParam.substr(nParamPos + 1, (strParam.size() - nParamPos));

			mapParams[strKey] = strValue;

			strHttpMsg.erase(0, nPos + 1);

			nPos = strHttpMsg.find('&');
		}

		return 0;
	}

	int CHttpParserBase::SendHttpErrorMsg(YTSvrLib::ITCPBASE* pSocket, UINT nRes, LPCSTR szMsgFormat, ...)
	{
		char msg[256] = { 0 };

		va_list va;
		va_start(va, szMsgFormat);
		__vsnprintf_s(msg, 255, szMsgFormat, va);
		va_end(va);

		char szRetMsg[512] = { 0 };

		_snprintf_s(szRetMsg, 511, "<root><result><ret>%d</ret><msg>%s</msg></result></root>", nRes, msg);

		return SendHttpMsg(pSocket, szRetMsg);
	}

	int CHttpParserBase::SendHttpMsg(YTSvrLib::ITCPBASE* pSocket, const char *pBuf)
	{
		if (pSocket == NULL)
		{
			return -1;
		}

		string strBody = pBuf;


#ifdef LIB_WINDOWS
		const char* pszHttpHead = "HTTP/1.1 200 OK\r\nServer: YTSVRLIB_HTTP_SERVER\r\nDate: %s GMT\r\nContent-Length: %d\r\nContent-Type: text/xml;charset=UTF-8\r\n\r\n";
#else
		const char* pszHttpHead = "HTTP/1.1 200 OK\nServer: YTSVRLIB_HTTP_SERVER\nDate: %s GMT\nContent-Length: %d\nContent-Type: text/xml;charset=UTF-8\n\n";
#endif

		time_t tNow = time(NULL);
		char szBufTime[128] = { 0 };
		tm tmUTC;
		ZeroMemory(&tmUTC, sizeof(tmUTC));
		gmtime_s(&tmUTC, &tNow);
		strftime(szBufTime, 127, "%a,%d %b %y %H:%M:%S", &tmUTC);

		char szHttpHead[512] = { 0 };
		_snprintf_s(szHttpHead, 511, pszHttpHead, szBufTime, strBody.size());

		string strPack = szHttpHead;
		strPack += strBody;

		pSocket->Send(strPack.c_str(), (int) strPack.size());
		pSocket->SafeClose();

		return (int) strPack.size();
	}

	void CEventHttpParser::PostRequest(evhttp_request* req)
	{
		AddToQueue(req);
	}

	void CEventHttpParser::OnMessageRecv()
	{
		evhttp_request* req = NULL;
		UINT nCount = 0;
		BOOL bNeedSetEvt = FALSE;
		for (;;)
		{
			m_qMsg.Lock();
			if (m_qMsg.empty())
			{
				m_qMsg.UnLock();
				break;
			}
			if (nCount >= 128)
			{	//避免其他事件队列堵塞
				bNeedSetEvt = TRUE;
				m_qMsg.UnLock();
				break;
			}
			req = m_qMsg.pop_front();
			m_qMsg.UnLock();

			ProcessMessage(req);
			nCount++;
		}
		if (bNeedSetEvt)
			SetEvent();
	}

	void CEventHttpParser::AddToQueue(evhttp_request* req)
	{
		BOOL bMustSetEvent = FALSE;
		m_qMsg.Lock();
		if (m_qMsg.empty())
		{
			bMustSetEvent = TRUE;
		}
		m_qMsg.push_back(req);
		m_qMsg.UnLock();
		if (bMustSetEvent)
		{
			SetEvent();
		}
	}
}