/*MIT License

Copyright (c) 2016 Zhe Xu

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

	void CURLWriter::Execute(void)
	{
		LOG("Start URL Writer Thread:%d", m_nID);
		CURL* pReq = curl_easy_init();
		curl_easy_setopt(pReq, CURLOPT_NOSIGNAL, 1);
		if (m_bShowHttpInfo)
		{
			curl_easy_setopt(pReq, CURLOPT_VERBOSE, 1);
		}

		while (TRUE)
		{
			CURLRequest* pReqInfo = m_pFactory->GetRequest();

			if (pReqInfo)
			{
				curl_easy_setopt(pReq, CURLOPT_URL, pReqInfo->m_strRequestURL.c_str());

				if (m_bIsPost)
				{
					curl_easy_setopt(pReq, CURLOPT_POST, true);
					curl_easy_setopt(pReq, CURLOPT_POSTFIELDS, pReqInfo->m_strPost.c_str());
				}

				curl_easy_perform(pReq);

				m_pFactory->ReleaseURLRequest(pReqInfo);
			}
		}

		curl_easy_cleanup(pReq);
	}

	BOOL CURLWriterFactory::StartURLWriter(UINT nCount, BOOL bShowHttpInfo, BOOL bIsPost /*= FALSE*/)
	{
		for (UINT i = 0; i < nCount; ++i)
		{
			CURLWriter* pWriter = new CURLWriter(this, i, bShowHttpInfo, bIsPost);

			if (pWriter)
			{
				AddThread(pWriter);
			}
		}

		Start(100);

		m_bInited = TRUE;

		return TRUE;
	}

	void CURLWriterFactory::AddURLRequest(LPCSTR lpszReq, LPCSTR lpszPost /*= ""*/)
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
		pReq->m_strPost = lpszPost;

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