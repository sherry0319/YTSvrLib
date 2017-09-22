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
#include "Application.h"

extern "C" {
	double get_version_code()
	{
		return YTSVRLIB_VERSION_CODE;
	}
}

void PureCallHandler(void)
{
	LOG("PureCallHandler");
	throw std::invalid_argument("");
}

void InvalidParameterHandler(const wchar_t* , const wchar_t* , const wchar_t* , unsigned int , uintptr_t )
{
	LOG("Invalid Parameter Handle");
	throw std::invalid_argument("");
}

UINT  g_nGlobalErrorCode;// 全局错误

namespace YTSvrLib
{
	void SetPrivateLastError(UINT nErrorCode)
	{
		g_nGlobalErrorCode = nErrorCode;
	}

	UINT GetPrivateLastError()
	{
		return g_nGlobalErrorCode;
	}

	CServerApplication::CServerApplication()
	{
#ifdef LIB_WINDOWS
		WSADATA      wsd;
		::WSAStartup(MAKEWORD((BYTE) 2, (BYTE) 2), &wsd);

		m_hException = NULL;

		m_hException = LoadLibraryA("XCPTHLR.dll");
		if (NULL == m_hException)
		{
			printf("LoadLibraryA XCPTHLR.dll Error=%d", GetLastError());
			getchar();
			exit(0);
		}
#endif // LIB_WINDOWS

		GlobalInit();
	}

	CServerApplication::~CServerApplication()
	{
		LOG("Server Cleaning");
#ifdef LIB_WINDOWS
		::WSACleanup();
		if (m_hException)
		{
			FreeLibrary(m_hException);
			m_hException = NULL;
		}
#endif // LIB_WINDOWS

		DelLogManager();
	}

	void CServerApplication::GlobalInit()
	{
		GetModuleFileName(m_szModuleName, MAX_PATH-1);

#ifdef LIB_WINDOWS
		char* pEnd = strrchr(m_szModuleName, '.');
		if (pEnd)
			*pEnd = '\0';
#endif // LIB_WINDOWS

		printf("m_szModuleName Final : %s\n", m_szModuleName);
		InitlogManager(m_szModuleName);

#ifdef LIB_WINDOWS
		srand(GetTickCount());
#else
		srand((UINT) time(NULL));
#endif // LIB_WINDOWS

#ifdef LIB_WINDOWS
		CWinsock ___do_not_touch_me;

		_set_invalid_parameter_handler(InvalidParameterHandler);
		_set_purecall_handler(PureCallHandler);
#endif // LIB_WINDOWS

		Init();
	}

	bool CServerApplication::Init()
	{
		//设置事件个数
		// _ASSERT(m_nEventCount<=APPLICATION_EVENT_MAX_COUNT);

		m_semQueue.Create(NULL, 0, INT_MAX);

		if (!InitEvent())
			return false;

		return true;
	}

	bool CServerApplication::InitEvent()
	{
		//初始化事件信息
		for (int i = 0; i < APPLICATION_EVENT_MAX_COUNT; ++i)
		{
			m_ayEventHandle[i].Proc = NULL;
			m_ayEventHandle[i].dwLastHandleTime = 0;
		}

		return true;
	}

	void CServerApplication::Run()
	{
		while (true)
		{
			m_semQueue.Lock();

			while (m_listEventQueue.size())
			{
				m_lockQueue.Lock();
				DWORD dwEvent = m_listEventQueue.front();
				m_listEventQueue.pop_front();
				m_lockQueue.UnLock();

				m_ayEventHandle[dwEvent].Proc();
				m_ayEventHandle[dwEvent].dwLastHandleTime = GetTickCount();
			}
		}
	}

	bool CServerApplication::CheckEventIndex(DWORD &dwEventIndex)
	{
		dwEventIndex += APPLICATION_EVENT_MIN_COUNT;
		if (dwEventIndex >= APPLICATION_EVENT_MAX_COUNT)
			return false;
		if (m_ayEventHandle[dwEventIndex].Proc)
		{
			LOG("Event ID repeated=%d", dwEventIndex);
			return false;
		}
		return true;
	}

	bool CServerApplication::SetEventInfo(DWORD dwEventIndex,EventProc Proc)
	{
		if (dwEventIndex >= APPLICATION_EVENT_MAX_COUNT)
			return false;

		m_ayEventHandle[dwEventIndex].dwLastHandleTime = 0;
		m_ayEventHandle[dwEventIndex].Proc = Proc;

		return true;
	}

	bool CServerApplication::RegisterEvent(DWORD dwEventIndex, EventProc Proc)
	{
		if (!CheckEventIndex(dwEventIndex))
			return false;

		return SetEventInfo(dwEventIndex, Proc);
	}

	void CServerApplication::SetEvent(DWORD dwEventIndex)
	{
		dwEventIndex += APPLICATION_EVENT_MIN_COUNT;
		if (dwEventIndex >= APPLICATION_EVENT_MAX_COUNT)
			return;

		m_lockQueue.Lock();
		BOOL bNeedSetEvent = FALSE;
		if (m_listEventQueue.size() == 0)
		{
			bNeedSetEvent = TRUE;
		}
		m_listEventQueue.push_back(dwEventIndex);
		m_lockQueue.UnLock();

		if (bNeedSetEvent)
		{
			m_semQueue.UnLock(1);
		}
	}
}