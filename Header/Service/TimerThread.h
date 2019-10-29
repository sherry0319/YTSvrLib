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
#ifndef __TIMER_THREAD_H_
#define __TIMER_THREAD_H_
#include <thread>
#include "../Socket/YTSocketThread.h"

namespace YTSvrLib
{
	class YTSVRLIB_EXPORT ITIMERTHREAD : public IASIOTHREAD
	{
	public:
		ITIMERTHREAD():IASIOTHREAD(),m_Timer(GetCore())
		{
			m_tExpireInMilsec = 0;
		}

		virtual ~ITIMERTHREAD();
		
		virtual void SetEvent() = 0;

		void OnTimer(bool set = false);

		BOOL CreateTimer(__time32_t tMilsecExpired);
	protected:
		asio::steady_timer m_Timer;
		__time32_t m_tExpireInMilsec;
	};
}

#endif // !__TIMER_THREAD_H_
