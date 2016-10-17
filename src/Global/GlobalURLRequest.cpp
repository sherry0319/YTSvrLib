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
#include "GlobalURLRequest.h"

namespace YTSvrLib
{

	CGlobalCURLRequest::CGlobalCURLRequest()
	{
		m_bCURLInited = FALSE;
		m_curlCore = NULL;
		Init();
	}

	CGlobalCURLRequest::~CGlobalCURLRequest()
	{
		m_bCURLInited = FALSE;
		m_curlCore = NULL;
	}

	void CGlobalCURLRequest::Init()
	{
		if (m_bCURLInited == FALSE)
		{
			curl_global_init(CURL_GLOBAL_ALL);
			m_bCURLInited = TRUE;
		}

		m_lockCore.Lock();

		if (m_curlCore == NULL)
		{
			m_curlCore = curl_easy_init();
		}

		m_lockCore.UnLock();
	}

	void CGlobalCURLRequest::SendHTTPGETMessage(const char* url, BOOL showinfo /*= TRUE*/)
	{
		if (m_bCURLInited == FALSE)
		{
			LOG("curl global not inited yet");
			return;
		}
		if (m_curlCore == NULL)
		{
			LOG("curl core is null");
			return;
		}

		m_lockCore.Lock();

		curl_easy_reset(m_curlCore);
		curl_easy_setopt(m_curlCore, CURLOPT_URL, url);
		curl_easy_setopt(m_curlCore, CURLOPT_NOSIGNAL, 1);

		if (showinfo)
		{
			curl_easy_setopt(m_curlCore, CURLOPT_VERBOSE, 1);
		}

		CURLcode emCode = curl_easy_perform(m_curlCore);

		m_lockCore.UnLock();

		if (CURLE_OK != emCode)
		{
			LOGERROR("error : curl request failed : %d\nrequest url : %s", emCode, url);
		}
	}

	void CGlobalCURLRequest::SendHTTPPOSTMessage(const char* url, const char* postdata, BOOL showinfo /*= TRUE*/)
	{
		if (m_bCURLInited == FALSE)
		{
			LOG("curl global not inited yet");
			return;
		}
		if (m_curlCore == NULL)
		{
			LOG("curl core is null");
			return;
		}

		int nPostLen = (int) strlen(postdata);

		m_lockCore.Lock();

		curl_easy_reset(m_curlCore);
		curl_easy_setopt(m_curlCore, CURLOPT_URL, url);
		curl_easy_setopt(m_curlCore, CURLOPT_POST, 1);
		curl_easy_setopt(m_curlCore, CURLOPT_POSTFIELDS, postdata);
		curl_easy_setopt(m_curlCore, CURLOPT_POSTFIELDSIZE, nPostLen);

		if (showinfo)
		{
			curl_easy_setopt(m_curlCore, CURLOPT_VERBOSE, 1);
		}

		CURLcode emCode = curl_easy_perform(m_curlCore);

		m_lockCore.UnLock();

		if (CURLE_OK != emCode)
		{
			LOGERROR("error : curl request failed : %d\nrequest url : %s\npostdata : %s", emCode, url, postdata);
		}
	}
}