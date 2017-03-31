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
		ITCPEVENTTHREAD::CleanThread();
		if (GetEvent())
		{
			event_base_loopexit(GetEvent(), NULL);
		}
	}

	void ITIMERTHREAD::OnTimer(evutil_socket_t, short, void* arg)
	{
		ITIMERTHREAD* pTimer = static_cast<ITIMERTHREAD*>(arg);
		if (pTimer)
		{
			pTimer->SetEvent();
		}
	}

	BOOL ITIMERTHREAD::CreateTimer(DWORD dwPeriod)
	{
		if (CreateEvent() == FALSE)
		{
			return FALSE;
		}

		event* timer_event = event_new(GetEvent(),(-1),EV_PERSIST,OnTimer,this);

		if (timer_event == NULL)
		{
			return FALSE;
		}

		long nPeriodSec = (long)(dwPeriod / 1000);
		long nPeriodNSec = (long)((dwPeriod % 1000) * 1000);
		timeval tPeriod = { nPeriodSec, nPeriodNSec };

		event_add(timer_event, &tPeriod);

		CreateThread();
	
		return TRUE;
	}
}