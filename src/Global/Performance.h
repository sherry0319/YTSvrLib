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
#pragma once

namespace YTSvrLib
{
#define PERFORMANCE_MICRO_SECONDS 1000000//微秒级定时
#define PERFORMANCE_MILLI_SECONDS 1000//毫秒级定时
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//性能检测器(默认为微秒)
#ifdef LIB_WINDOWS
	class YTSVRLIB_EXPORT PerformanceWatch
	{
	public:
		PerformanceWatch(size_t nTimesize);

		~PerformanceWatch(){}
	private:
		size_t m_nTimesize;
	private:
		LARGE_INTEGER m_begintick;
		LARGE_INTEGER m_endtick;
		LARGE_INTEGER m_costtick;
		LARGE_INTEGER m_lasttick;
	public:
		LONGLONG Start();

		LONGLONG Stop();

		LONGLONG GetCostTotal();

		LONGLONG CheckPoint(LPCSTR lpszInfo, ...);
	};
#else
	class PerformanceWatch
	{
	public:
		PerformanceWatch(int nTimesize);
		~PerformanceWatch(){}
	private:
		size_t m_nTimesize;
	private:
		timeval m_begintick;
		timeval m_endtick;
		timeval m_lasttick;
	public:
		void Start();

		LONGLONG Stop();

		LONGLONG GetSpan(timeval& begin, timeval& end);

		LONGLONG GetCostTotal();

		void CheckPoint(LPCSTR lpszInfo, ...);
	};
#endif // LIB_WINDOWS

	// 开始一个本地性能检测
#define BEGIN_WATCH(type) YTSvrLib::PerformanceWatch pw(type);pw.Start()
	// 通过检查点
#define CHECK_WATCH(s, ...) pw.CheckPoint(s, __VA_ARGS__)
	// 结束本地性能测试
#define END_WATCH() pw.Stop()
}