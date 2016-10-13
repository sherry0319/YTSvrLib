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

#pragma once

#define YTSVRLIB_VERSION_CODE	1.0

#define APPLICATION_EVENT_MAX_COUNT 16	//主线程最多处理事件个数，该值不能超过64
#define APPLICATION_EVENT_MIN_COUNT 1	//主线程最少设置事件个数，预留用于通用事件

#define USER_EVENT_MAX_COUNT	(APPLICATION_EVENT_MAX_COUNT-APPLICATION_EVENT_MIN_COUNT)	//用户最多可定义的事件个数
#define EVENT_OVERTIME_CHECK_SPACE	50	//事件未触发时间超过该值时,会重新检测,单位毫秒

extern "C" {
	double get_version_code();
}

namespace YTSvrLib
{

	typedef void(*EventProc)();//事件处理函数类型定义

	typedef struct _EVENTINFO
	{
		EventProc	Proc;		//事件处理函数
		DWORD		dwLastHandleTime;//最后一次处理时间

		_EVENTINFO() :/*dwPeriod(0),*/ dwLastHandleTime(0)
		{}
	}EVENTINFO, *PEVENTINFO;

	class CServerApplication
	{
	public:
		explicit CServerApplication();
		~CServerApplication();
		//系统初始化，由main()调用，nEventCount为事件个数(必须不超过USER_EVENT_MAX_COUNT)， 返回false则系统启动失败
		//pAppExitEventName: 关闭程序时,设置该名称的事件,由监控程序设置
		void GlobalInit();
		bool Init(int nEventCount, const char* pAppExitEventName);
		void Run();//主线程进入阻塞状态，用于等待及处理事件，由main()调用

		//================================注册事件及处理函数================================
		bool RegisterEvent(DWORD dwEventIndex, EventProc Proc);//设置事件处理函数
		void SetEvent(DWORD dwEventIndex);//激活事件，索引参数必须有效，尽量减少该函数的调用次数（比如有网络消息达到，尽量只在消息队列为空情况下调用）
	private:
		bool InitEvent();//初始化事件，设置公共事件
		bool CheckEventIndex(DWORD &dwEventIndex);//对dwEventIndex转换，并校验dwEventIndex的合法性，对已重复设置的句柄执行关闭操作
		bool SetEventInfo(DWORD dwEventIndex, HANDLE hEventHandle, EventProc Proc);
	private:
		int						m_nEventCount;	//事件个数
		CHAR					m_szModuleName[MAX_PATH];//进程名称
		EVENTINFO					m_ayEventHandle[APPLICATION_EVENT_MAX_COUNT];// 事件处理器

#ifdef LIB_WINDOWS
		HMODULE					m_hException;
		HMODULE					m_hZlib;
#endif // LIB_WINDOWS

		std::list<DWORD>		m_listEventQueue;// 事件队列
		YTSvrLib::CLock			m_lockQueue;// 事件队列锁
		YTSvrLib::CSemaphore	m_semQueue;// 事件队列信号量
	};
}

