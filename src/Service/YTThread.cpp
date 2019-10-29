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
#include "YTThread.h"

namespace YTSvrLib
{

	CThread::CThread(void) : m_bTerminated(FALSE), m_dwThreadID(0)
	{
#ifdef LIB_WINDOWS
		m_hThread = INVALID_HANDLE_VALUE;
#endif // LIB_WINDOWS

	}

	CThread::~CThread(void)
	{
		if (IsStarted())
		{
			Terminate();
			WaitFor();
		}
	}

	void CThread::WaitFor(void)
	{
		//to do: handle SendMessage deadlock
#ifdef LIB_WINDOWS
		if (m_hThread != INVALID_HANDLE_VALUE)
		{
			WaitForSingleObject(m_hThread, INFINITE);
		}
#else
		if (m_dwThreadID > 0)
		{
			void* pThreadRet = NULL;
			pthread_join(m_dwThreadID, &pThreadRet);
		}
#endif // LIB_WINDOWS

	}

	BOOL CThread::Start()
	{
#ifdef LIB_WINDOWS
		UINT nThreadID = 0;
		HANDLE hThread = (HANDLE) _beginthreadex(NULL,
												 0,
												 ThreadProc,
												 (void*)this,
												 0,
												 &nThreadID);
		if (hThread == INVALID_HANDLE_VALUE)
			return FALSE;

		m_hThread = hThread;
		m_dwThreadID = (DWORD)nThreadID;
#else
		int ret = pthread_create(&m_dwThreadID, NULL, ThreadProc, (void*)this);
		if (ret != 0)
		{
			return FALSE;
		}
#endif // LIB_WINDOWS

		m_bTerminated = FALSE;

		return TRUE;
	}

	BOOL CThread::IsTerminated(void)
	{
		return m_bTerminated;
	}

	BOOL CThread::IsStarted(void)
	{
#ifdef LIB_WINDOWS
		if (m_hThread == NULL || m_hThread == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
#else
		if (m_dwThreadID == 0 || m_dwThreadID == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
#endif // LIB_WINDOWS

		return TRUE;
	}

	void CThread::Terminate(void)
	{
		//ODS("FSDAF");
#ifdef LIB_WINDOWS
		::InterlockedExchange((LONG *) &m_bTerminated, TRUE);
#else
		m_bTerminated = TRUE;
#endif // LIB_WINDOWS
	}

#ifdef LIB_WINDOWS
	unsigned WINAPI CThread::ThreadProc(LPVOID pParam)
	{
		CThread* pThread = (CThread*) pParam;

		if (!pThread->PreLoopFunc())
		{
			::InterlockedExchange((LONG *) &(pThread->m_bTerminated), TRUE);
			return 0;
		}
		try
		{
			pThread->Execute();
		}
		catch (...)
		{
		}
		try
		{
			pThread->OnTerminated();
		}
		catch (...)
		{
		}
		CloseHandle(pThread->m_hThread);
		pThread->m_hThread = INVALID_HANDLE_VALUE;
		pThread->m_dwThreadID = 0;
		pThread->m_bTerminated = TRUE;
		return 0;
	}
#else
	void* CThread::ThreadProc(LPVOID pParam)
	{
		pthread_detach(pthread_self());

		BlockSignal();

		CThread* pThread = (CThread*) pParam;

		if (!pThread->PreLoopFunc())
		{
			pThread->m_bTerminated = TRUE;
			return NULL;
		}
		try
		{
			pThread->Execute();
		}
		catch (...)
		{
			return NULL;
		}
		try
		{
			pThread->OnTerminated();
		}
		catch (...)
		{
			return NULL;
		}
		pThread->m_dwThreadID = 0;
		pThread->m_bTerminated = TRUE;
		pthread_exit(NULL);

		return pThread;
	}
#endif // LIB_WINDOWS

	///在添加了线程之后启动管理
	void CThreadPool::Start(DWORD dwSleep /*= 0*/)
	{
		for (unsigned int i = 0; i < m_vecThreads.size(); i++)
		{
			CThread* pThread = m_vecThreads[i];
			if (!pThread->IsStarted())
			{
				pThread->Start();
				Sleep(dwSleep);
			}
		}
	}

	///终止管理，并停止所有线程
	void CThreadPool::ShutDown()
	{
		for (unsigned int i = 0; i < m_vecThreads.size(); i++)
			m_vecThreads[i]->Terminate();
		while (m_vecThreads.size() > 0)
		{
			CThread* pThread = m_vecThreads[0];
			pThread->WaitFor();
			delete pThread;
			m_vecThreads.erase(m_vecThreads.begin());
		}
	}

	CThread* CThreadPool::GetThread(size_t nIndex)
	{
		if (nIndex >= m_vecThreads.size())
			return NULL;
		return m_vecThreads[nIndex];
	}
}