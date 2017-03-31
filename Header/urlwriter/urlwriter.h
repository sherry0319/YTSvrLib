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
#ifndef _URL_WRITER_H
#define _URL_WRITER_H

namespace YTSvrLib
{

	struct CURLRequest : public CRecycle
	{
		std::string m_strRequestURL;
		std::string m_strPost;
	};

	class CURLWriterFactory : public CThreadPool
	{
	public:
		CURLWriterFactory() :m_poolURLRequest("CURLRequest")
		{
			m_bInited = FALSE;
			m_semLock.Create(NULL, 0, MAXLONG);
		}

		virtual ~CURLWriterFactory()
		{
			m_bInited = FALSE;
		}

		BOOL StartURLWriter(UINT nCount, BOOL bShowHttpInfo, BOOL bIsPost = FALSE);

		void AddURLRequest(LPCSTR lpszReq, LPCSTR lpszPost = "");

		CURLRequest* GetRequest();

		void ReleaseURLRequest(CURLRequest* pReq);

		void WaitForAllRequestDone();

		UINT GetCurListSize()
		{
			return (UINT) m_listUrlRequest.size();
		};
	protected:
		std::list<CURLRequest*> m_listUrlRequest;
		YTSvrLib::CSemaphore m_semLock;
		YTSvrLib::CLock m_criLock;
		BOOL m_bInited;
		CPool<CURLRequest, 128> m_poolURLRequest;
	};

	class CURLWriter : public CThread
	{
	public:

		CURLWriter(CURLWriterFactory* pFactory, UINT nID, BOOL bShowHttpInfo, BOOL bIsPost) :
			m_nID(nID), m_bShowHttpInfo(bShowHttpInfo), m_bIsPost(bIsPost), m_pFactory(pFactory)
		{

		}

		virtual ~CURLWriter()
		{

		}

	public:
		virtual void Execute(void);

	protected:
		UINT m_nID;
		BOOL m_bShowHttpInfo;
		BOOL m_bIsPost;
		CURLWriterFactory* m_pFactory;
	};

}

#endif