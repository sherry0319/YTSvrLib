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
#include "stdafx.h"
#include "SyncObj.h"

namespace YTSvrLib
{
#ifdef LIB_WINDOWS


#else
	CSpinLock::CSpinLock()
	{
		pthread_spin_init(&m_lock,0);
	}

	CSpinLock::~CSpinLock()
	{
		pthread_spin_destroy(&m_lock);
	}

	BOOL CSpinLock::Lock()
	{
		pthread_spin_lock(&m_lock);
	}

	void CSpinLock::UnLock()
	{
		pthread_spin_unlock(&m_lock);
	}

	CCriticalSection::CCriticalSection()
	{
		pthread_mutex_init(&m_cs, NULL);
	}

	CCriticalSection::~CCriticalSection()
	{
		pthread_mutex_destroy(&m_cs);
	}

	BOOL CCriticalSection::Lock()
	{
		pthread_mutex_lock(&m_cs);

		return  TRUE;
	}

	BOOL CCriticalSection::Trylock(void)
	{
		if (pthread_mutex_trylock(&m_cs) == 0)
		{
			return TRUE;
		}

		return FALSE;
	}

	void CCriticalSection::UnLock()
	{
		pthread_mutex_unlock(&m_cs);
	}

	CRecursiveLock::CRecursiveLock(void)
	{
		pthread_mutexattr_init(&m_csAttr);

		pthread_mutexattr_settype(&m_csAttr, PTHREAD_MUTEX_RECURSIVE_NP);

		pthread_mutex_init(&m_cs, &m_csAttr);
	}

	CRecursiveLock::~CRecursiveLock(void)
	{
		pthread_mutex_destroy(&m_cs);
	}

	BOOL CRecursiveLock::Lock(void)
	{
		pthread_mutex_lock(&m_cs);

		return TRUE;
	}

	void CRecursiveLock::UnLock(void)
	{
		pthread_mutex_unlock(&m_cs);
	}

	CSemaphore::~CSemaphore()
	{
		sem_destroy(&m_Sem);
	}

	BOOL CSemaphore::Lock(DWORD dwTimeOut /* = INFINITE */)
	{
		if (dwTimeOut == INFINITE)
		{
			if (sem_wait(&m_Sem))
			{
				return FALSE;
			}
		}
		else
		{
#define TIME_USEC	(1000000)
			timeval tv;
			timespec ts;

			gettimeofday(&tv,NULL);

			int nSec = (int) (dwTimeOut / 1000);
			int nMilSec = dwTimeOut - (nSec * 1000);

			tv.tv_sec += nSec;
			tv.tv_usec += (nMilSec * 1000);

			if (tv.tv_usec > TIME_USEC)
			{
				tv.tv_sec += (long)(tv.tv_usec / TIME_USEC);
				tv.tv_usec %= TIME_USEC;
			}
			
			ts.tv_sec = tv.tv_sec;
			ts.tv_nsec = (tv.tv_usec * 1000);

			if (sem_timedwait(&m_Sem, &ts))
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	BOOL CSemaphore::UnLock(LONG lCount)
	{
		if (sem_post(&m_Sem))
		{
			printf_s("Sem Unlock Failed : %d", errno);
			return FALSE;
		}
		return TRUE;
	}

	BOOL CSemaphore::Create(WCHAR *pName, LONG lInitCount, LONG lMaxCount)
	{
		int nRet = sem_init(&m_Sem, 0, 0);
		if (nRet)
		{
			printf_s("Semaphore Create Failed : %d .errno : %d", nRet, errno);
			return FALSE;
		}

		UnLock(lInitCount);

		return TRUE;
	}
#endif // LIB_WINDOWS
}