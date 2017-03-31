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

	const size_t THREAD_INFO_LEN = 128;

	class CThread //: public CRecycle
	{
	public:
		CThread(void);
		virtual ~CThread(void);
		///启动线程
		virtual BOOL Start();

		///终止线程
		void Terminate(void);

		///判断线程是否已终止
		BOOL IsTerminated(void);

		///线程是否已启动
		BOOL IsStarted(void);

		///等待线程退出
		void WaitFor(void);

		const char * GetThreadInfo()
		{
			return (char *) &m_ThreadInfo;
		}

	protected:
		///记录线程类型的缓冲，当线程异常退出方便查找问题
		char    m_ThreadInfo[THREAD_INFO_LEN];
		virtual void SetThreadInfo()
		{};
	protected:

		virtual bool PreLoopFunc()
		{
			return true;
		}
		///线程的主运行体
		virtual void Execute(void) = 0;

		///Terminate事件
		virtual void OnTerminated(void)
		{};

		///标志是否已终止
		volatile BOOL m_bTerminated;

	private:
		//线程ID
		DWORD m_dwThreadID;
#ifdef LIB_WINDOWS
		///标志是否已挂起
		BOOL m_bIsSuspended;
		///线程句柄
		HANDLE m_hThread;
#endif // LIB_WINDOWS

#ifdef LIB_WINDOWS
		static unsigned WINAPI ThreadProc(LPVOID pParam);
#else
		static void* ThreadProc(LPVOID pParam);
#endif // LIB_WINDOWS
	};

	/**
	@brief 管理多个线程对象。
	*
	*	管理多个线程对象。
	*/

	class CThreadPool
	{
	public:
		CThreadPool(void)
		{}
		virtual ~CThreadPool(void)
		{
			if (m_vecThreads.size() > 0)
				ShutDown();
		}

		///在添加了线程之后启动管理
		void Start(DWORD dwSleep = 0);

		///终止管理，并停止所有线程
		void ShutDown();

		///获取当前的线程个数
		size_t GetThreadCount()
		{
			return m_vecThreads.size();
		}

		///增加一个线程对象
		void AddThread(CThread* pThread)
		{
			if (pThread) m_vecThreads.push_back(pThread);
		}

		///获取一个线程对象
		CThread* GetThread(size_t nIndex);
	private:
		///已创建的线程管理类列表
		std::vector<CThread*> m_vecThreads;
	};
}