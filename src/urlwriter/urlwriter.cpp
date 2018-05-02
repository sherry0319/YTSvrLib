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
#include "urlwriter.h"

namespace YTSvrLib
{
	size_t CURLWriterClient::URLDataWriter(void* data, size_t size, size_t nmemb, void* content)
	{
		size_t nTotalSize = size*nmemb;
		if (data == NULL)
		{
			return nTotalSize;
		}
		std::string* pstrRequest = (std::string*)content;
		if (pstrRequest)
		{
			pstrRequest->append((char *) data, ((char*) data) + nTotalSize);
		}
		return nTotalSize;
	}

	void CURLWriterClient::Execute(void)
	{
		LOG("Start URL Writer Thread:%d", m_nID);
		CURL* pReq = curl_easy_init();
		curl_easy_setopt(pReq, CURLOPT_NOSIGNAL, 1);

		while (TRUE)
		{
			CURLRequest* pReqInfo = m_pFactory->GetRequest();

			if (pReqInfo)
			{
				curl_easy_setopt(pReq, CURLOPT_URL, pReqInfo->m_strRequestURL.c_str());

				if (!pReqInfo->m_strPost.empty())
				{
					curl_easy_setopt(pReq, CURLOPT_POST, true);
					curl_easy_setopt(pReq, CURLOPT_POSTFIELDS, pReqInfo->m_strPost.c_str());
				} 
				if (pReqInfo->m_nReadTimeout)
				{
					curl_easy_setopt(pReq, CURLOPT_TIMEOUT, (LONG)pReqInfo->m_nReadTimeout);
				}
				curl_easy_setopt(pReq, CURLOPT_WRITEFUNCTION, CURLWriterClient::URLDataWriter);
				curl_easy_setopt(pReq, CURLOPT_WRITEDATA, (void*) &pReqInfo->m_strReturn);

				CURLcode emCode = curl_easy_perform(pReq);

				if (emCode == CURLE_OK)
				{
					curl_easy_getinfo(pReq, CURLINFO_RESPONSE_CODE, &pReqInfo->m_nReturnCode);
				}

				if (pReqInfo->m_pFuncCallBack)
				{
					m_pFactory->GetCallBackLock();
					pReqInfo->m_pFuncCallBack(pReqInfo);
					m_pFactory->ReleaseCallBackLock();
				}
				
				m_pFactory->ReleaseURLRequest(pReqInfo);
			}
		}

		curl_easy_cleanup(pReq);
	}

	BOOL CURLWriterFactory::StartURLWriter(UINT nCount)
	{
		for (UINT i = 0; i < nCount; ++i)
		{
			CURLWriterClient* pWriter = new CURLWriterClient(this, i);

			if (pWriter)
			{
				AddThread(pWriter);
			}
		}

		Start(100);

		m_bInited = TRUE;

		return TRUE;
	}

	void CURLWriterFactory::AddURLRequest(LPCSTR lpszReq, URLPARAM nParam1 /*= 0*/, URLPARAM nParam2 /*= 0*/, URLPARAM nParam3 /*= 0*/, URLPARAM nParam4 /*= 0*/, 
										  _REQUEST_CALLBACK pFunction /*= NULL*/, int nReadTimeout /*= 0*/)
	{
		AddURLRequest(lpszReq, "", nParam1, nParam2, nParam3, nParam4, pFunction, nReadTimeout);
	}

	void CURLWriterFactory::AddURLRequest(LPCSTR lpszReq, LPCSTR lpszPost, 
										  URLPARAM nParam1 /*= 0*/, URLPARAM nParam2 /*= 0*/, URLPARAM nParam3 /*= 0*/, URLPARAM nParam4 /*= 0*/, 
										  _REQUEST_CALLBACK pFunction /*= NULL*/, int nReadTimeout /*= 0*/)
	{
		if (m_bInited == FALSE)
		{
			LOG("CURLWriterMgr need init before use");
			return;
		}
		m_criLock.Lock();

		CURLRequest* pReq = m_poolURLRequest.ApplyObj();
		if (pReq == NULL)
		{
			m_criLock.UnLock();
			return;
		}
		pReq->m_strRequestURL = lpszReq;
		if (lpszPost)
		{
			pReq->m_strPost = lpszPost;
		}
		pReq->m_ayParam[0] = nParam1;
		pReq->m_ayParam[1] = nParam2;
		pReq->m_ayParam[2] = nParam3;
		pReq->m_ayParam[3] = nParam4;
		pReq->m_nReadTimeout = nReadTimeout;
		pReq->m_pFuncCallBack = pFunction;

		m_listUrlRequest.push_back(pReq);

		m_criLock.UnLock();

		m_semLock.UnLock(1);
	}

	CURLRequest* CURLWriterFactory::GetRequest()
	{
		m_semLock.Lock();

		m_criLock.Lock();

		CURLRequest* pReq = NULL;
		if (m_listUrlRequest.size())
		{
			pReq = m_listUrlRequest.front();
			m_listUrlRequest.pop_front();
		}

		m_criLock.UnLock();

		return pReq;
	}

	void CURLWriterFactory::ReleaseURLRequest(CURLRequest* pReq)
	{
		m_poolURLRequest.ReclaimObj(pReq);
	}

	void CURLWriterFactory::GetCallBackLock()
	{
		m_criLockCallback.Lock();
	}

	void CURLWriterFactory::ReleaseCallBackLock()
	{
		m_criLockCallback.UnLock();
	}

	void CURLWriterFactory::WaitForAllRequestDone()
	{
		while (TRUE)
		{
			size_t nLeft = m_listUrlRequest.size();

			if (nLeft > 10000)
			{
				Sleep(1000);
			}
			else if (nLeft > 0)
			{
				Sleep(100);
			}
			else
			{
				break;
			}
		}
	}
}