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
#include "YTSocketThread.h"
#include <event2/thread.h>

namespace YTSvrLib
{
	BOOL ITCPEVENTTHREAD::CreateEvent()
	{
#ifdef LIB_WINDOWS
		evthread_use_windows_threads();
#else
		evthread_use_pthreads();
#endif // LIB_WINDOWS

		m_eventbase = event_base_new();
		if (m_eventbase == NULL)
		{
			LOGERROR("event_base_new failed : %d",GetLastError());
			return FALSE;
		}

		evthread_make_base_notifiable(m_eventbase);

		return TRUE;
	}

	void ITCPEVENTTHREAD::StopEvent()
	{
		if (m_eventbase)
		{
			event_base_free(m_eventbase);
			m_eventbase = NULL;
		}
	}

	void ITCPEVENTTHREAD::CreateThread()
	{
		m_bRun = TRUE;

		m_Thread = std::thread(&ITCPEVENTTHREAD::EventLoop,this);

		m_Thread.detach();
	}

	void ITCPEVENTTHREAD::CleanThread()
	{
		StopEvent();
		m_bRun = FALSE;
	}

	void ITCPEVENTTHREAD::StopThread()
	{
		m_bRun = FALSE;
		if (GetEvent())
		{
			event_base_loopexit(GetEvent(), NULL);
		}
	}

	void ITCPEVENTTHREAD::EventLoop()
	{
#ifndef LIB_WINDOWS
		pthread_detach(pthread_self());

		BlockSignal();
#endif
		if (m_eventbase == NULL)
		{
			CreateEvent();
		}

		m_bRuning = TRUE;

		while (m_bRun)
		{
			try
			{
				if (m_eventbase)
				{
					event_base_dispatch(m_eventbase);
					Sleep(1000);
				}
				else
					m_bRun = FALSE;
			}
			catch (...)
			{
				m_bRun = FALSE;
			}
		}

		m_bRuning = FALSE;

		OnThreadEnd();
	}
}