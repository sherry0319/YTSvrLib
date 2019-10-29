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

namespace YTSvrLib
{
	void ITHREAD::CreateThread(LPCSTR threadName)
	{
		if (m_bRun)
		{
			return;
		}
		m_bRun = TRUE;
		strncpy_s(m_szThreadName, threadName, 63);

		std::thread t(&ITHREAD::Run, this);

		t.detach();
	}

	void ITHREAD::StopThread()
	{
		m_bRun = FALSE;
	}

	void ITHREAD::CleanThread()
	{
		m_bRun = FALSE;
	}

	void ITHREAD::Run() {
#ifndef LIB_WINDOWS
		pthread_detach(pthread_self());

		BlockSignal();
#endif
		EventLoop();
	}

	void IASIOTHREAD::StopThread() {
		m_bRun = FALSE;

		if (!io.stopped())
		{
			io.stop();
		}
	}

	void IASIOTHREAD::EventLoop() {
		while (m_bRun)
		{
			try
			{
				io.run();
			}
			catch (...)
			{
				m_bRun = FALSE;
			}
			// LOG("Thread Stoped : Name=[%s]", m_szThreadName);
			Sleep(THREAD_DELAY_MSEC);
		}
	}
}