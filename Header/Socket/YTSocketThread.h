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
#ifndef __YTSOCKET_THREAD_H_
#define __YTSOCKET_THREAD_H_

#include <thread>
#include <event.h>

#ifdef YTLIB_WITH_WEBSOCKET
#include <asio.hpp>
#include <asio/io_service.hpp>
#endif // YTLIB_WITH_WEBSOCKET

namespace YTSvrLib
{
	class YTSVRLIB_EXPORT ITHREAD {
	public:
		ITHREAD() {
			m_bRun = FALSE;
			m_bRuning = FALSE;
		}

		BOOL IsRuning() const
		{
			return m_bRuning;
		}

		void CreateThread();

		void Run();

		virtual void EventLoop() = 0;

		virtual void OnThreadEnd() {}// 线程结束

		virtual void StopThread();

		virtual void CleanThread();
	protected:
		BOOL m_bRun;
		BOOL m_bRuning;
	};

	class YTSVRLIB_EXPORT ITCPEVENTTHREAD : public ITHREAD
	{
	public:
		ITCPEVENTTHREAD() : ITHREAD()
		{
			m_eventbase = NULL;
		}

		virtual ~ITCPEVENTTHREAD()
		{

		}

		BOOL CreateEvent();

		event_base* GetEvent()
		{
			return m_eventbase;
		}

		virtual void EventLoop(); // 线程函数
	protected:
		virtual void CleanThread() override;

		virtual void StopThread() override;

		void StopEvent();
	private:
		event_base* m_eventbase;
	};

#ifdef YTLIB_WITH_WEBSOCKET
	class YTSVRLIB_EXPORT IASIOTHREAD : public ITHREAD {
	public:
		IASIOTHREAD() : ITHREAD()
		{}

		asio::io_service* GetCore() {
			return &io;
		}

		virtual void EventLoop(); // 线程函数
	private:
		asio::io_service io;
	};
#endif // YTLIB_WITH_WEBSOCKET
}

#endif // !__YTSOCKET_THREAD_H_
