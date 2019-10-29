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
#include "TimerThread.h"

namespace YTSvrLib
{
	ITIMERTHREAD::~ITIMERTHREAD()
	{
		StopThread();
	}

	void ITIMERTHREAD::OnTimer(bool set /*= false*/)
	{
		if (set)
		{
			SetEvent();
		}

		m_Timer.expires_after(std::chrono::milliseconds(m_tExpireInMilsec - THREAD_DELAY_MSEC));// 定时器减去线程延迟才是准确的定时时间

		m_Timer.async_wait(std::bind(&ITIMERTHREAD::OnTimer, this, true));
	}

	BOOL ITIMERTHREAD::CreateTimer(__time32_t tMilsecExpired)
	{
		m_tExpireInMilsec = tMilsecExpired;

		OnTimer(false);

		CreateThread("ITIMERTHREAD");
	
		return TRUE;
	}
}