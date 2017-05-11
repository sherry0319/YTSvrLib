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
#include "Perfermance.h"

namespace YTSvrLib
{
#ifdef LIB_WINDOWS
	PerformanceWatch::PerformanceWatch(size_t nTimesize)
	{
		m_nTimesize = nTimesize;
		m_begintick.QuadPart = 0;
		m_endtick.QuadPart = 0;
		m_costtick.QuadPart = 0;
		QueryPerformanceFrequency(&m_costtick);
	}

	LONGLONG PerformanceWatch::Start()
	{
		QueryPerformanceCounter(&m_begintick);
		m_lasttick = m_begintick;

		LONGLONG llStart = (m_begintick.QuadPart % m_costtick.QuadPart) * m_nTimesize / (m_costtick.QuadPart);

		return llStart;
	}

	LONGLONG PerformanceWatch::Stop()
	{
		QueryPerformanceCounter(&m_endtick);

		LONGLONG llStop = (m_endtick.QuadPart % m_costtick.QuadPart) * m_nTimesize / (m_costtick.QuadPart);

		return llStop;
	}

	LONGLONG PerformanceWatch::GetCostTotal()
	{
		LONGLONG cost = m_endtick.QuadPart - m_begintick.QuadPart;

		LONGLONG ms = (cost*m_nTimesize) / m_costtick.QuadPart;

		return ms;
	}

	LONGLONG PerformanceWatch::CheckPoint(LPCSTR lpszInfo, ...)
	{
		LARGE_INTEGER check;

		QueryPerformanceCounter(&check);

		LONGLONG cost_total = check.QuadPart - m_begintick.QuadPart;

		LONGLONG ms_total = (cost_total*m_nTimesize) / m_costtick.QuadPart;

		//////////////////////////////////////////////////////////////////////////
		LONGLONG cost_check = check.QuadPart - m_lasttick.QuadPart;

		LONGLONG ms_check = (cost_check*m_nTimesize) / m_costtick.QuadPart;

		m_lasttick = check;


		char szInfo[2048] = { 0 };

		va_list va;
		va_start(va, lpszInfo);

		vsprintf_s(szInfo, 2047, lpszInfo, va);

		va_end(va);

		sprintf_s(szInfo, 2047, "%s[LastPoint(%lld),BeginPoint(%lld)]", szInfo, ms_check, ms_total);

		LOG(szInfo);

		return ms_check;
	}
#else
	PerformanceWatch::PerformanceWatch(int nTimesize)
	{
		m_nTimesize = nTimesize;
		ZeroMemory(&m_begintick, sizeof(m_begintick));
		ZeroMemory(&m_endtick, sizeof(m_endtick));
		ZeroMemory(&m_lasttick, sizeof(m_lasttick));
	}

	void PerformanceWatch::Start()
	{
		gettimeofday(&m_begintick, NULL);

		m_lasttick.tv_sec = m_begintick.tv_sec;
		m_lasttick.tv_usec = m_begintick.tv_usec;
	}

	LONGLONG PerformanceWatch::Stop()
	{
		gettimeofday(&m_endtick, NULL);
	}

	LONGLONG PerformanceWatch::GetSpan(timeval& begin, timeval& end)
	{
		int nSec = end.tv_sec - begin.tv_sec;
		int nUsec = 0;

		if (end.tv_usec >= begin.tv_usec)
		{
			nUsec = end.tv_usec - begin.tv_usec;
		}
		else
		{
			nSec--;
			nUsec = ((PERFERMANCE_MICRO_SECONDS - begin.tv_usec) + end.tv_usec);
		}

		if (m_nTimesize == PERFERMANCE_MICRO_SECONDS)
		{
			return ((nSec * PERFERMANCE_MICRO_SECONDS) + nUsec);
		}
		else
		{
			return ((nSec * PERFERMANCE_MILLI_SECONDS) + (LONGLONG) (nUsec / PERFERMANCE_MILLI_SECONDS));
		}
	}

	LONGLONG PerformanceWatch::GetCostTotal()
	{
		return GetSpan(m_begintick, m_endtick);
	}

	void PerformanceWatch::CheckPoint(LPCSTR lpszInfo, ...)
	{
		timeval check;
		gettimeofday(&check, NULL);

		LONGLONG ms_total = GetSpan(m_begintick, check);

		LONGLONG ms_check = GetSpan(m_lasttick, check);

		m_lasttick = check;

		char szInfo[2048] = { 0 };

		va_list va;
		va_start(va, lpszInfo);
		vsprintf_s(szInfo, 2047, lpszInfo, va);
		va_end(va);

		sprintf_s(szInfo, 2047, "%s[LastPoint(%lld),BeginPoint(%lld)]", szInfo, ms_check, ms_total);

		LOG(szInfo);
	}
#endif


}