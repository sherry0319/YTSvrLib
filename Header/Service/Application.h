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

/////////////////////////////////////////////////////////////////////////////
// CServerApplication : Core Instance.服务器进程的唯一入口以及事件主循环
//

#pragma once

#define YTSVRLIB_VERSION_CODE	1.01

#define APPLICATION_EVENT_MAX_COUNT 16	//main thread event max count.主线程事件最大数量
#define APPLICATION_EVENT_MIN_COUNT 1	//main thread event min count.主线程事件最小数量

#define USER_EVENT_MAX_COUNT	(APPLICATION_EVENT_MAX_COUNT-APPLICATION_EVENT_MIN_COUNT)	//max user event count.用户可自定义的事件数量

namespace YTSvrLib
{
	YTSVRLIB_EXPORT void SetPrivateLastError(UINT nErrorCode);
	YTSVRLIB_EXPORT UINT GetPrivateLastError();

	typedef void(*EventProc)();//event handle function define

	typedef struct _EVENTINFO
	{
		EventProc	Proc;		//event handler
		DWORD		dwLastHandleTime;//last handle time.最后一次处理时间
		_EVENTINFO() :dwLastHandleTime(0){
			Proc = NULL;
		}
	}EVENTINFO, *PEVENTINFO;

	class YTSVRLIB_EXPORT CServerApplication : public CSingle<CServerApplication>
	{
	public:
		explicit CServerApplication();
		virtual ~CServerApplication();

		// call it to enter event loop.waiting for event to handle.make sure all config has been prepaired.主线程进入阻塞状态，用于等待及处理事件，由main()调用.调用之前确保配置都已经准备完毕.
		void Run();

		// regist a event to event loop 设置事件处理函数
		bool RegisterEvent(DWORD dwEventIndex, EventProc Proc);

		// add a new event to event loop.and activate the event.
		void SetEvent(DWORD dwEventIndex);
	private:
		// 初始化服务器
		void GlobalInit();

		// set the event count .init the event pool.
		bool Init();

		// init event pool.
		bool InitEvent();

		// check the dwEventIndex available.
		bool CheckEventIndex(DWORD &dwEventIndex);

		// set the event.
		bool SetEventInfo(DWORD dwEventIndex, EventProc Proc);
	private:
		CHAR					m_szModuleName[MAX_PATH];//process name.进程名
		EVENTINFO					m_ayEventHandle[APPLICATION_EVENT_MAX_COUNT];// event handle map. 事件表

#ifdef LIB_WINDOWS
		HMODULE					m_hException;
#endif // LIB_WINDOWS

		std::list<DWORD>		m_listEventQueue;// event queue.事件队列
		YTSvrLib::CLock			m_lockQueue;// lock for event queue.事件队列锁
		YTSvrLib::CSemaphore	m_semQueue;// sem for event queue.事件队列信号量
	};
}

