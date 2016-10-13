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
#pragma	   once
#ifdef LIB_WINDOWS
#pragma comment( lib, "Kernel32.lib" )
#else
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sem.h>
#endif // LIB_WINDOWS


namespace YTSvrLib
{
#ifdef LIB_WINDOWS
	class CCriticalSection
	{
	public:
		explicit CCriticalSection(void)
		{
			InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
		}
		~CCriticalSection(void)
		{
			DeleteCriticalSection(&m_cs);
		}

		BOOL Lock(void)
		{
			EnterCriticalSection(&m_cs);
			return  TRUE;
		}

		void UnLock(void)
		{
			LeaveCriticalSection(&m_cs);
		}

	private:
		CRITICAL_SECTION m_cs;
	};

	typedef CCriticalSection CLock;

	class   CMutex
	{
	public:
		CMutex() :m_hMutex(NULL)
		{}

		CMutex(bool bAccess, WCHAR* pName) :m_hMutex(NULL)
		{
			Create(bAccess, pName);
		}

		virtual ~CMutex()
		{
			if (NULL != m_hMutex)
			{
				CloseHandle(m_hMutex);
			}
		}

		BOOL    Create(bool bAccess, WCHAR *pName)
		{
			if ((m_hMutex = CreateMutex(NULL, bAccess, pName)) == NULL)
			{
				return FALSE;
			}
			return  TRUE;
		}

		BOOL    Open(bool bAccess, WCHAR *pName)
		{
			if ((m_hMutex = OpenMutex(bAccess, FALSE, pName)) == NULL)
			{
				return FALSE;
			}
			return  TRUE;
		}

		BOOL    Lock(DWORD dwTimeOut = INFINITE)
		{
			DWORD  dwReturn = WaitForSingleObject(m_hMutex, dwTimeOut);
			if (dwReturn == WAIT_OBJECT_0)
			{
				return TRUE;
			}
			return  FALSE;
		}

		BOOL    UnLock()
		{
			return  ReleaseMutex(m_hMutex);
		}
	private:

		HANDLE  m_hMutex;

	protected:
	};

	class   CEvent
	{
	public:
		CEvent() :m_hEvent(NULL)
		{}
		CEvent(BOOL bManualReset, BOOL bInitialState, WCHAR * pName) :m_hEvent(NULL)
		{
			Create(bManualReset, bInitialState, pName);
		}
		virtual ~CEvent()
		{
			if (m_hEvent != NULL)
			{
				CloseHandle(m_hEvent);
			}
		}

		BOOL  Create(BOOL bManualReset, BOOL bInitialState, WCHAR * pName)
		{
			m_bManualReset = bManualReset;
			if ((m_hEvent = CreateEvent(NULL, bManualReset, bInitialState, pName)) == NULL)
			{
				return  FALSE;
			}
			return  TRUE;
		}

		BOOL  Lock(DWORD  dwTimeOut = INFINITE)
		{
			DWORD  dwReturn = WaitForSingleObject(m_hEvent, dwTimeOut);
			if (dwReturn == WAIT_OBJECT_0)
			{
				return TRUE;
			}
			return  FALSE;
		}

		BOOL  UnLock(void)
		{
			//»Àπ§÷ÿ÷√
			if (m_bManualReset)
			{
				PulseEvent(m_hEvent);
			}
			else
			{
				SetEvent(m_hEvent);
			}
		}
	private:

		HANDLE  m_hEvent;
		BOOL    m_bManualReset;

	protected:
	};

	class   CSemaphore
	{
	public:

		CSemaphore(void) :m_hSemaphore(NULL)
		{}
		virtual ~CSemaphore(void)
		{
			if (m_hSemaphore != NULL)
			{
				CloseHandle(m_hSemaphore);
			}
		}

		BOOL   Create(WCHAR *pName, LONG lInitCount, LONG lMaxCount)
		{
			m_hSemaphore = CreateSemaphore(NULL, lInitCount, lMaxCount, pName);
			if (m_hSemaphore == NULL)
			{
				return FALSE;
			}
			return  TRUE;
		}

		BOOL   Lock(DWORD  dwTimeOut = INFINITE)
		{

			DWORD dwReturn = WaitForSingleObject(m_hSemaphore, dwTimeOut);
			if (dwReturn == WAIT_OBJECT_0)
			{
				return TRUE;
			}
			return  FALSE;
		}

		BOOL   UnLock(LONG lCount)
		{
			return ReleaseSemaphore(m_hSemaphore, lCount, NULL);
		}

	private:

		HANDLE m_hSemaphore;

	protected:
	};
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <semaphore.h>

	class CCriticalSection
	{
	public:
		//CCriticalSection(const CCriticalSection& rSrc);
		//void operator=(const CCriticalSection& rSrc);
		explicit CCriticalSection(void);
		~CCriticalSection(void);
		BOOL Lock(void);
		BOOL Trylock(void);
		void UnLock(void);
	private:
		pthread_mutex_t m_cs;
	};
	typedef CCriticalSection CLock;
	typedef CCriticalSection CMutex;
	typedef CCriticalSection CEvent;

	class CRecursiveLock
	{
	public:
		explicit CRecursiveLock(void);
		~CRecursiveLock(void);
		BOOL Lock(void);
		void UnLock(void);
	private:
		pthread_mutex_t m_cs;
		pthread_mutexattr_t m_csAttr;
	};

	class CSpinLock
	{
	public:
		explicit CSpinLock(void);
		~CSpinLock(void);

		BOOL Lock(void);
		void UnLock(void);
	private:
		pthread_spinlock_t m_lock;
	};

	class CSemaphore
	{
	public:

		CSemaphore(void){}
		virtual ~CSemaphore(void);

		BOOL Lock(DWORD  dwTimeOut = INFINITE);

		BOOL UnLock(LONG lCount);

		BOOL Create(WCHAR *pName, LONG lInitCount, LONG lMaxCount);

	private:
		sem_t m_Sem;
	};
#endif // LIB_WINDOWS
}
