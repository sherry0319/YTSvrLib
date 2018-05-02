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
	struct CURLRequest;
	
	typedef LONGLONG URLPARAM;

	typedef void(*_REQUEST_CALLBACK)(CURLRequest* pReq);

	struct YTSVRLIB_EXPORT CURLRequest : public CRecycle
	{
		virtual void Init()
		{
			m_strRequestURL.clear();
			m_strRequestURL.shrink_to_fit();
			m_strPost.clear();
			m_strPost.shrink_to_fit();
			m_strReturn.clear();
			m_strReturn.shrink_to_fit();

			m_nReturnCode = 0;
			m_pFuncCallBack = NULL;
			ZeroMemory(m_ayParam, sizeof(m_ayParam));
			m_nReadTimeout = 0;
		}

		std::string m_strRequestURL;
		std::string m_strPost;
		int m_nReturnCode;
		std::string m_strReturn;
		URLPARAM m_ayParam[4];
		int m_nReadTimeout;
		_REQUEST_CALLBACK m_pFuncCallBack;
	};

	class YTSVRLIB_EXPORT CURLWriterFactory : public CThreadPool
	{
	public:
		friend class CURLWriterClient;
		CURLWriterFactory() :m_poolURLRequest("CURLRequest")
		{
			m_bInited = FALSE;
			m_semLock.Create(NULL, 0, MAXLONG);
		}

		virtual ~CURLWriterFactory()
		{
			m_bInited = FALSE;
		}

		// Start a mutithread url writer.
		BOOL StartURLWriter(UINT nCount);

		// Send a request.nParam 1-4 will send back in callback function.
		// _REQUEST_CALLBACK is defined as void _REQUEST_CALLBACK(YTSvrLib::CURLRequest*)
		// Be careful the callback function will be called by mutithread in a single queue.
		// And the param YTSvrLib::CURLRequest* passed by the callback will be released immediately after the callback function returned.
		// You should copy the data by yourself if you want to use it after callback returned. 
		void AddURLRequest(LPCSTR lpszReq, LPCSTR lpszPost, URLPARAM nParam1 = 0, URLPARAM nParam2 = 0, URLPARAM nParam3 = 0, URLPARAM nParam4 = 0, 
						   _REQUEST_CALLBACK pFunction = NULL, int nReadTimeout = 0);
		void AddURLRequest(LPCSTR lpszReq, URLPARAM nParam1 = 0, URLPARAM nParam2 = 0, URLPARAM nParam3 = 0, URLPARAM nParam4 = 0, 
						   _REQUEST_CALLBACK pFunction = NULL, int nReadTimeout = 0);

		void WaitForAllRequestDone();

		UINT GetCurListSize()
		{
			return (UINT) m_listUrlRequest.size();
		}
	protected:
		CURLRequest* GetRequest();

		void ReleaseURLRequest(CURLRequest* pReq);

		void GetCallBackLock();

		void ReleaseCallBackLock();
	protected:
		std::list<CURLRequest*> m_listUrlRequest;
		YTSvrLib::CSemaphore m_semLock;
		YTSvrLib::CLock m_criLock;
		BOOL m_bInited;
		CPool<CURLRequest, 128> m_poolURLRequest;
	protected:
		YTSvrLib::CLock m_criLockCallback;
	};

	class YTSVRLIB_EXPORT CURLWriterClient : public CThread
	{
	public:

		CURLWriterClient(CURLWriterFactory* pFactory, UINT nID) : m_nID(nID), m_pFactory(pFactory)
		{

		}

		virtual ~CURLWriterClient()
		{
			m_nID = 0;
			m_pFactory = NULL;
		}

	public:
		virtual void Execute(void);

		static size_t URLDataWriter(void* data, size_t size, size_t nmemb, void* content);
	protected:
		UINT m_nID;
		CURLWriterFactory* m_pFactory;
	};

}

#endif