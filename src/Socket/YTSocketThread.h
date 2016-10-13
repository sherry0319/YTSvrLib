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
#ifndef __YTSOCKET_THREAD_H_
#define __YTSOCKET_THREAD_H_

#include <thread>
#include <event.h>

namespace YTSvrLib
{
	class ITCPEVENTTHREAD
	{
	public:
		ITCPEVENTTHREAD()
		{
			m_eventbase = NULL;
			m_bRun = FALSE;
			m_bRuning = FALSE;
		}

		virtual ~ITCPEVENTTHREAD()
		{

		}

		BOOL CreateEvent();

		BOOL IsRuning() const
		{
			return m_bRuning;
		}

		void CreateThread();

		event_base* GetEvent()
		{
			return m_eventbase;
		}

		virtual void EventLoop(); // 线程函数

		virtual void OnThreadEnd(){}// 线程结束
	protected:
		virtual void CleanThread();

		void StopThread();

		void StopEvent();
	private:
		event_base* m_eventbase;
		std::thread m_Thread;
		BOOL m_bRun;
		BOOL m_bRuning;
	};
}

#endif // !__YTSOCKET_THREAD_H_
