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

BOOL	g_bOpenLog = TRUE;
void OpenLog()
{
	g_bOpenLog = TRUE;
}
void CloseLog()
{
	g_bOpenLog = FALSE;
}
BOOL IsOpenLog()
{
	return g_bOpenLog;
}

void LogBin(const char* pszData, int nDataLen)
{
	LOG("BinData Len=%d", nDataLen);
	char szTmp[128];
	int nLen = 0;
	UINT nCount = 0;
	for (int i = 0; i < nDataLen; i++)
	{
		nLen += __snprintf_s(szTmp + nLen, 127 - nLen, "%02X ", (BYTE) pszData[i]);

		nCount++;
		if (nCount >= 16)
		{
			YTSvrLib::CLogBufferA* pLogBuffer = YTSvrLib::CProcessLogMgr::GetInstance()->ApplyObj();
			char* lpszWriteBuf = pLogBuffer->GetBuffer();
			pLogBuffer->ReSize(
				sprintf_s(lpszWriteBuf, pLogBuffer->GetCapcity(), "[%s]\r\n", szTmp)
				);
			YTSvrLib::CLogManager::PostBufferReady(YTSvrLib::CProcessLogMgr::GetInstance(), pLogBuffer, YTSvrLib::IOErrorData);
			nCount = 0;
			nLen = 0;
		}
	}
	if (nLen > 0)
	{
		YTSvrLib::CLogBufferA* pLogBuffer = YTSvrLib::CProcessLogMgr::GetInstance()->ApplyObj();
		char* lpszWriteBuf = pLogBuffer->GetBuffer();
		pLogBuffer->ReSize(
			sprintf_s(lpszWriteBuf, pLogBuffer->GetCapcity(), "[%s]\r\n", szTmp)
			);
		YTSvrLib::CLogManager::PostBufferReady(YTSvrLib::CProcessLogMgr::GetInstance(), pLogBuffer, YTSvrLib::IOErrorData);
	}
}

void LogErrorASync(const char* szFile, const int nLine, EM_LOG_LEVEL emLevel, const char *fmt, ...)
{
	char cTime[128] = { 0 };
	GetDateTime(cTime, 127, 'S');
	cTime[127] = '\0';

	char lpszBuf[10240] = { 0 };
	va_list	ap;
	va_start(ap, fmt);
	vsprintf_s(lpszBuf, _countof(lpszBuf), fmt, ap);
	va_end(ap);

	const char* pszLevel = "DEBUG";
	switch (emLevel)
	{
		case LOG_LEVEL_TRACE:
			pszLevel = "TRACE";
			break;
		case LOG_LEVEL_WARN:
			pszLevel = "WARN";
			break;
		case LOG_LEVEL_ERROR:
			pszLevel = "ERROR";
			break;
		default:
			break;
	}

	char* pszFileShort = strrchr((char*) szFile, '\\');
	if (pszFileShort)
		pszFileShort++;
	if (pszFileShort == NULL)
		pszFileShort = (char*) szFile;

	YTSvrLib::CLogBufferA* pLogBuffer = YTSvrLib::CProcessLogMgr::GetInstance()->ApplyObj();
	char* lpszWriteBuf = pLogBuffer->GetBuffer();
	pLogBuffer->ReSize(
		sprintf_s(lpszWriteBuf, pLogBuffer->GetCapcity(), "[%s][%s,tid=0x%04x][%s:%d] %s%s",
		pszLevel,
		cTime, 
#ifdef LIB_WINDOWS
		GetCurrentThreadId()
#else
		pthread_self()
#endif // LIB_WINDOWS
		, pszFileShort, nLine, lpszBuf,
#ifdef LIB_WINDOWS
		"\r\n"
#else
		"\n"
#endif
)
		);
	_ASSERT(strlen(lpszWriteBuf));
	switch (emLevel)
	{
		case LOG_LEVEL_TRACE:
			break;
		case LOG_LEVEL_WARN:
		case LOG_LEVEL_ERROR:
		{
#ifdef LIB_WINDOWS
			cout << pLogBuffer->GetBuffer();
#endif
			cerr << pLogBuffer->GetBuffer();
		}
			break;
		default:
		{
#ifdef LIB_WINDOWS
			cout << pLogBuffer->GetBuffer();
#endif
		}
			break;
	}
	YTSvrLib::CLogManager::PostBufferReady(YTSvrLib::CProcessLogMgr::GetInstance(), pLogBuffer, YTSvrLib::IOErrorData);
}

// void LogError(const char* szFile, const int nLine, const char *fmt, ...)
// {
// 	char cTime[128] = { 0 };
// 	GetDateTime(cTime, 127, 'S');
// 	cTime[127] = '\0';
// 
// 	char lpszBuf[10240] = { 0 };
// 	va_list	ap;
// 	va_start(ap, fmt);
// 	vsprintf_s(lpszBuf, _countof(lpszBuf), fmt, ap);
// 	va_end(ap);
// 
// 	char* pszFileShort = strrchr((char*) szFile, '\\');
// 	if (pszFileShort)
// 		pszFileShort++;
// 	if (pszFileShort == NULL)
// 		pszFileShort = (char*) szFile;
// 
// 	YTSvrLib::CLogBufferA LogBuffer;
// 	char* lpszWriteBuf = LogBuffer.GetBuffer();
// 	LogBuffer.ReSize(
// 		sprintf_s(lpszWriteBuf, LogBuffer.GetCapcity(), "[%s,tid=0x%04x][%s:%d] %s\r\n",
// 		cTime, 
// #ifdef LIB_WINDOWS
// 		GetCurrentThreadId()
// #else
// 		pthread_self()
// #endif // LIB_WINDOWS
// 		, pszFileShort, nLine, lpszBuf)
// 		);
// 	YTSvrLib::CLogManager::WriteSynLog(YTSvrLib::CProcessLogMgr::GetInstance(), &LogBuffer);
// }

void LogCommon(const char *fmt, ...)
{
	char cCurDate[32] = { 0 };
	GetDateTime(cCurDate, 32, 'D');
	char cCurTime[32] = { 0 };
	GetDateTime(cCurTime, 32, 'T');

	YTSvrLib::CLogBufferA* pLogBuffer = YTSvrLib::CProcessLogMgr::GetInstance()->ApplyObj();
	char* lpszWriteBuf = pLogBuffer->GetBuffer();
	va_list	ap;
	va_start(ap, fmt);
	pLogBuffer->ReSize(
		vsprintf_s(lpszWriteBuf, pLogBuffer->GetCapcity(), fmt, ap)
		);
	va_end(ap);
	YTSvrLib::CLogManager::PostBufferReady(YTSvrLib::CProcessLogMgr::GetInstance(), pLogBuffer, YTSvrLib::IOCommonData);
}

void ReOpenLogFile()
{
	YTSvrLib::CLogManager::PostBufferReady(YTSvrLib::CProcessLogMgr::GetInstance(), NULL, YTSvrLib::IONewDay);
	YTSvrLib::CProcessLogMgr::GetInstance()->ReOpenSynLogFile();
}

namespace YTSvrLib
{

#ifdef LIB_WINDOWS
	CLogManager::CLogManager() : m_pool("CLogManager")
	{
		m_hThread = INVALID_HANDLE_VALUE;
		m_hIOCP = INVALID_HANDLE_VALUE;
		m_hAsynFileHandle = INVALID_HANDLE_VALUE;
		m_hSynFileHandle = INVALID_HANDLE_VALUE;
		m_pool.SetLogEnable(FALSE);
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
		_ASSERT(m_hIOCP != NULL);
		m_szFilePrefix[0] = '\0';
		m_szFileExt[0] = '\0';
		m_bInited = FALSE;
		m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadIOCPProc, (void*)this, 0, 0);
	}

	CLogManager::~CLogManager()
	{
		CloseHandle(m_hAsynFileHandle); CloseHandle(m_hIOCP); CloseHandle(m_hThread); CloseHandle(m_hSynFileHandle);
	};

	void CLogManager::Init(LPCSTR pszPrefix)
	{
		if (m_bInited == FALSE)
		{
			if (pszPrefix && *pszPrefix != '\0')
			{
				strncpy_s(m_szFilePrefix, pszPrefix, FILE_PREFIX_MAXLEN - 1);
				m_szFilePrefix[FILE_PREFIX_MAXLEN - 1] = '\0';
			}
			else
				m_szFilePrefix[0] = '\0';
			m_hAsynFileHandle = OpenAsynLogFile(m_wzFilePath);
			m_hSynFileHandle = OpenSynLogFile();
			m_bInited = TRUE;
		}
	};

	BOOL CLogManager::PostBufferReady(CLogManager* pLogMgr, CLogBufferA* pBuffer, EM_IO_TYPE eIO)
	{
		HANDLE hIOCP = pLogMgr->GetIOCPHandle();
		BOOL fOk = PostQueuedCompletionStatus(hIOCP, 0, eIO, pBuffer);
		_ASSERT(fOk);
		return(fOk);
	}

	void CLogManager::WriteSynLog(CLogManager* pLogMgr, CLogBufferA* pBuffer)
	{
		pLogMgr->LockSyncFile();
		cout << pBuffer->GetBuffer();
		DWORD dwIOSize = 0;
		HANDLE hFileHandle = pLogMgr->GetSyncFileHandle();
		if (hFileHandle != INVALID_HANDLE_VALUE)
			WriteFile(hFileHandle, pBuffer->GetBuffer(),
			pBuffer->GetSize(), &dwIOSize, /*(OVERLAPPED*)pLogBuffer*/NULL);
		pLogMgr->UnlockSyncFile();
	}

	void CLogManager::ReOpenSynLogFile()
	{
		m_lockSynFile.Lock();
		CloseHandle(m_hSynFileHandle);
		m_hSynFileHandle = OpenSynLogFile();
		m_lockSynFile.UnLock();
	}

	void CLogManager::SetFilePrefix(LPCSTR pszPrefix)
	{
		strncpy_s(m_szFilePrefix, pszPrefix, FILE_PREFIX_MAXLEN - 1);
		m_szFilePrefix[FILE_PREFIX_MAXLEN - 1] = '\0';
	}

	BOOL CLogManager::AssociateDevice(HANDLE hDevice, EM_IO_TYPE eIO)
	{
		BOOL fOk = (CreateIoCompletionPort(hDevice, m_hIOCP, eIO, 0) == m_hIOCP);
		_ASSERT(fOk);
		return(fOk);
	}

	void CLogManager::Write(EM_IO_TYPE /*eIO*/, CLogBufferA* pLogBuffer, PLARGE_INTEGER pliOffset /*= NULL*/)
	{
		if (pliOffset != NULL)
		{
			pLogBuffer->Offset = pliOffset->LowPart;
			pLogBuffer->OffsetHigh = pliOffset->HighPart;
		}
		else
		{
			_ASSERT(0);
		}
		DWORD dwIOSize = 0;
		WriteFile( m_hAsynFileHandle, pLogBuffer->GetBuffer(),pLogBuffer->GetSize(), &dwIOSize, NULL);
		m_pool.ReclaimObj(pLogBuffer);
	}

	void CLogManager::ReOpenAsynLogFile()
	{
		CloseHandle(m_hAsynFileHandle);
		m_hAsynFileHandle = OpenAsynLogFile(m_wzFilePath);
	}

	unsigned WINAPI CLogManager::ThreadIOCPProc(LPVOID pParam)
	{
		CLogManager* pThis = (CLogManager*) pParam;

		LARGE_INTEGER liNextWriteOffset = { 0 };
		LARGE_INTEGER liCurrWriteOffset = { 0 };

		HANDLE hIOCP = pThis->GetIOCPHandle();
		LPOVERLAPPED pOL = 0;
		CLogBufferA* pLogBuffer = 0;
		EM_IO_TYPE eIO;
		DWORD dwError = 0;
		DWORD dwIOSize = 0;
		for (;;)
		{
			pLogBuffer = 0;
			ULONG_PTR nIOType = 0;
			BOOL bIoRet = GetQueuedCompletionStatus(
				hIOCP,
				&dwIOSize,
				&nIOType,//(PULONG_PTR) &eIO,
				&pOL, INFINITE);
			eIO = (EM_IO_TYPE) nIOType;
			if (pOL == NULL)
			{
				if (eIO == IONewDay)
				{
					pThis->ReOpenAsynLogFile();
					continue;
				}
				cout << "LogThread OnExit..." << endl;
				break;
			}
			pLogBuffer = (CLogBufferA*) pOL;

			if (bIoRet)
			{
				if (eIO == IOErrorData || eIO == IOCommonData)
				{
					//printf( pLogBuffer->GetBuffer() );
					pThis->Write(eIO, pLogBuffer, &liNextWriteOffset);
				}
				else if (eIO == IOWrite)
				{
					//pThis->m_pool.ReclaimObj( pLogBuffer );
					liCurrWriteOffset.QuadPart += dwIOSize;

					if (liCurrWriteOffset.QuadPart > liNextWriteOffset.QuadPart)
					{
						//printf( "Error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
					}
					else if (liCurrWriteOffset.QuadPart == liNextWriteOffset.QuadPart)
					{
						//printf( "\n\n------------------------Mach----------------------\n\n" );
					}
					else
					{
						//printf( "                 Slow            \n" );
					}
				}
			}
			else
			{
				dwError = GetLastError();
				if (dwError == WAIT_TIMEOUT)
				{
					//WriteAll();
				}
			}
		}
		return 0;
	}
	HANDLE CLogManager::OpenSynLogFile()
	{
		char lpszCurDir[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, lpszCurDir, MAX_PATH);
		char* pEnd = strrchr(lpszCurDir, '\\');
		if (pEnd)
			*pEnd = '\0';
		strcat_s(lpszCurDir, 256, "\\");
		strcat_s(lpszCurDir, 256, LOG_FILE_SYN);
		strcat_s(lpszCurDir, 256, "\\");

		SYSTEMTIME time;
		ZeroMemory(&time, sizeof(SYSTEMTIME));
		GetLocalTime(&time);

		if (m_szFilePrefix[0] != '\0')
		{
			sprintf_s(lpszCurDir, 256, ("%s%d-%.2d\\%s\\"), lpszCurDir, time.wYear, time.wMonth, m_szFilePrefix);
		}
		else
		{
			sprintf_s(lpszCurDir, 256, ("%s%d-%.2d\\"), lpszCurDir, time.wYear, time.wMonth);
		}

		if (m_szFileExt[0] == '\0')
		{
			strncpy_s(m_szFileExt, "log", 15);
		}

		if (!MakeSureDirectoryPathExists(lpszCurDir))
		{
			return INVALID_HANDLE_VALUE;
		}
		if (m_szFilePrefix[0] != '\0')
			sprintf_s(lpszCurDir, ("%s[%s]%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-syn.%s"), lpszCurDir, m_szFilePrefix,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, m_szFileExt);
		else
			sprintf_s(lpszCurDir, ("%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-syn.%s"), lpszCurDir,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, m_szFileExt);

		HANDLE hTemp = CreateFileA(lpszCurDir, GENERIC_WRITE,
								   FILE_SHARE_READ, NULL, CREATE_ALWAYS,
								   FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_OVERLAPPED*/, 0);
		return hTemp;
	}

	HANDLE CLogManager::OpenAsynLogFile(const char* pstrFileName)
	{
		char lpszCurDir[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, lpszCurDir, MAX_PATH);
		char* pEnd = strrchr(lpszCurDir, '\\');
		if (pEnd)
			*pEnd = '\0';
		strcat_s(lpszCurDir, 256, "\\");
		strcat_s(lpszCurDir, 256, pstrFileName);
		strcat_s(lpszCurDir, 256, "\\");

		DWORD dwProcessID = GetCurrentProcessId();

		SYSTEMTIME time;
		ZeroMemory(&time, sizeof(SYSTEMTIME));
		GetLocalTime(&time);

		if (m_szFilePrefix[0] != '\0')
		{
			sprintf_s(lpszCurDir, 256, ("%s%d-%.2d\\%s\\"), lpszCurDir, time.wYear, time.wMonth, m_szFilePrefix);
		}
		else
		{
			sprintf_s(lpszCurDir, 256, ("%s%d-%.2d\\"), lpszCurDir, time.wYear, time.wMonth);
		}

		if (m_szFileExt[0] == '\0')
		{
			strncpy_s(m_szFileExt, "log", 15);
		}

		if (!MakeSureDirectoryPathExists(lpszCurDir))
		{
			return INVALID_HANDLE_VALUE;
		}
		if (m_szFilePrefix[0] != '\0')
			sprintf_s(lpszCurDir, ("%s%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-[%d]%s.%s"), lpszCurDir, m_szFilePrefix,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, dwProcessID, pstrFileName, m_szFileExt);
		else
			sprintf_s(lpszCurDir, ("%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-[%d]%s.%s"), lpszCurDir,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, dwProcessID, pstrFileName, m_szFileExt);

		HANDLE hTemp = CreateFileA(lpszCurDir, GENERIC_WRITE,
								   FILE_SHARE_READ, NULL, CREATE_ALWAYS,
								   FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_OVERLAPPED*/, 0);
		return hTemp;
	}

	void CLogManager::ShutDown()
	{
		LOG("LogManager ShutDown..");
		PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);

		WaitForMultipleObjects(1, &m_hThread, TRUE, 10000);
		if (m_hThread != NULL && m_hThread != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hThread);
			m_hThread = INVALID_HANDLE_VALUE;
		}
		if (m_hIOCP != NULL && m_hIOCP != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hIOCP);
			m_hIOCP = INVALID_HANDLE_VALUE;
		}
		if (m_hSynFileHandle != NULL && m_hSynFileHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hSynFileHandle);
			m_hSynFileHandle = INVALID_HANDLE_VALUE;
		}
	}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CLogManagerW Windows

	CLogManagerW::CLogManagerW() : m_pool("CLogManagerW")
	{
		m_hThread = INVALID_HANDLE_VALUE;
		m_hIOCP = INVALID_HANDLE_VALUE;
		m_hAsynFileHandle = INVALID_HANDLE_VALUE;
		m_hSynFileHandle = INVALID_HANDLE_VALUE;
		m_pool.SetLogEnable(FALSE);
		m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
		_ASSERT(m_hIOCP != NULL);
		m_szFilePrefix[0] = '\0';
		m_szFileExt[0] = '\0';
		m_bInited = FALSE;
		m_hThread = (HANDLE) _beginthreadex(NULL, 0, ThreadIOCPProc, (void*)this, 0, 0);
	}

	CLogManagerW::~CLogManagerW()
	{
		CloseHandle(m_hAsynFileHandle); CloseHandle(m_hIOCP); CloseHandle(m_hThread); CloseHandle(m_hSynFileHandle);
	};

	void CLogManagerW::Init(LPCSTR pszPrefix)
	{
		if (m_bInited == FALSE)
		{
			if (pszPrefix && *pszPrefix != '\0')
			{
				strncpy_s(m_szFilePrefix, pszPrefix, FILE_PREFIX_MAXLEN - 1);
				m_szFilePrefix[FILE_PREFIX_MAXLEN - 1] = '\0';
			}
			else
				m_szFilePrefix[0] = '\0';
			m_hAsynFileHandle = OpenAsynLogFile(m_wzFilePath);
			m_hSynFileHandle = OpenSynLogFile();
			m_bInited = TRUE;
		}
	};

	BOOL CLogManagerW::PostBufferReady(CLogManagerW* pLogMgr, CLogBufferW* pBuffer, EM_IO_TYPE eIO)
	{
		HANDLE hIOCP = pLogMgr->GetIOCPHandle();
		BOOL fOk = PostQueuedCompletionStatus(hIOCP, 0, eIO, pBuffer);
		_ASSERT(fOk);
		return(fOk);
	}

	void CLogManagerW::WriteSynLog(CLogManagerW* pLogMgr, CLogBufferW* pBuffer)
	{
		pLogMgr->LockSyncFile();
		wprintf(pBuffer->GetBuffer());
		DWORD dwIOSize = 0;
		HANDLE hFileHandle = pLogMgr->GetSyncFileHandle();
		if (hFileHandle != INVALID_HANDLE_VALUE)
			WriteFile(hFileHandle, pBuffer->GetBuffer(),(pBuffer->GetSize() << 1), &dwIOSize, NULL);
		pLogMgr->UnlockSyncFile();
	}

	void CLogManagerW::ReOpenSynLogFile()
	{
		m_lockSynFile.Lock();
		CloseHandle(m_hSynFileHandle);
		m_hSynFileHandle = OpenSynLogFile();
		m_lockSynFile.UnLock();
	}

	void CLogManagerW::SetFilePrefix(LPCSTR pszPrefix)
	{
		strncpy_s(m_szFilePrefix, pszPrefix, FILE_PREFIX_MAXLEN - 1);
		m_szFilePrefix[FILE_PREFIX_MAXLEN - 1] = '\0';
	}

	BOOL CLogManagerW::AssociateDevice(HANDLE hDevice, EM_IO_TYPE eIO)
	{
		BOOL fOk = (CreateIoCompletionPort(hDevice, m_hIOCP, eIO, 0) == m_hIOCP);
		_ASSERT(fOk);
		return(fOk);
	}

	void CLogManagerW::Write(EM_IO_TYPE /*eIO*/, CLogBufferW* pLogBuffer, PLARGE_INTEGER pliOffset /*= NULL*/)
	{
		if (pliOffset != NULL)
		{
			pLogBuffer->Offset = pliOffset->LowPart;
			pLogBuffer->OffsetHigh = pliOffset->HighPart;
		}
		else
		{
			_ASSERT(0);
		}

		DWORD dwIOSize = 0;
		WriteFile( m_hAsynFileHandle, pLogBuffer->GetBuffer(),(DWORD) (pLogBuffer->GetSize() << 1), &dwIOSize,NULL);

		m_pool.ReclaimObj(pLogBuffer);
	}

	void CLogManagerW::ReOpenAsynLogFile()
	{
		CloseHandle(m_hAsynFileHandle);
		m_hAsynFileHandle = OpenAsynLogFile(m_wzFilePath);
	}

	unsigned WINAPI CLogManagerW::ThreadIOCPProc(LPVOID pParam)
	{
		CLogManagerW* pThis = (CLogManagerW*) pParam;

		LARGE_INTEGER liNextWriteOffset = { 0 };
		LARGE_INTEGER liCurrWriteOffset = { 0 };

		HANDLE hIOCP = pThis->GetIOCPHandle();
		LPOVERLAPPED pOL = 0;
		CLogBufferW* pLogBuffer = 0;
		EM_IO_TYPE eIO;
		DWORD dwError = 0;
		DWORD dwIOSize = 0;
		for (;;)
		{
			pLogBuffer = 0;
			ULONG_PTR nIOType = 0;
			BOOL bIoRet = GetQueuedCompletionStatus(
				hIOCP,
				&dwIOSize,
				&nIOType,//(PULONG_PTR) &eIO,
				&pOL, INFINITE);
			eIO = (EM_IO_TYPE) nIOType;
			if (pOL == NULL)
			{
				if (eIO == IONewDay)
				{
					pThis->ReOpenAsynLogFile();
					continue;
				}
				cout << "LogThread OnExit..." << endl;
				break;
			}
			pLogBuffer = (CLogBufferW*) pOL;

			if (bIoRet)
			{
				if (eIO == IOErrorData || eIO == IOCommonData)
				{
					//printf( pLogBuffer->GetBuffer() );
					pThis->Write(eIO, pLogBuffer, &liNextWriteOffset);
				}
				else if (eIO == IOWrite)
				{
					//pThis->m_pool.ReclaimObj( pLogBuffer );
					liCurrWriteOffset.QuadPart += dwIOSize;

					if (liCurrWriteOffset.QuadPart > liNextWriteOffset.QuadPart)
					{
						//printf( "Error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" );
					}
					else if (liCurrWriteOffset.QuadPart == liNextWriteOffset.QuadPart)
					{
						//printf( "\n\n------------------------Mach----------------------\n\n" );
					}
					else
					{
						//printf( "                 Slow            \n" );
					}
				}
			}
			else
			{
				dwError = GetLastError();
				if (dwError == WAIT_TIMEOUT)
				{
					//WriteAll();
				}
			}
		}
		return 0;
	}
	HANDLE CLogManagerW::OpenSynLogFile()
	{
		char lpszCurDir[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, lpszCurDir, MAX_PATH);
		char* pEnd = strrchr(lpszCurDir, '\\');
		if (pEnd)
			*pEnd = '\0';
		strcat_s(lpszCurDir, 256, "\\");
		strcat_s(lpszCurDir, 256, LOG_FILE_SYN);
		strcat_s(lpszCurDir, 256, "\\");

		SYSTEMTIME time;
		ZeroMemory(&time, sizeof(SYSTEMTIME));
		GetLocalTime(&time);

		if (m_szFilePrefix[0] != '\0')
		{
			sprintf_s(lpszCurDir, 256, ("%s%d-%.2d\\%s\\"), lpszCurDir, time.wYear, time.wMonth, m_szFilePrefix);
		}
		else
		{
			sprintf_s(lpszCurDir, 256, ("%s%d-%.2d\\"), lpszCurDir, time.wYear, time.wMonth);
		}

		if (!MakeSureDirectoryPathExists(lpszCurDir))
		{
			return INVALID_HANDLE_VALUE;
		}

		if (m_szFileExt[0] == '\0')
		{
			strncpy_s(m_szFileExt, "log", 15);
		}

		if (m_szFilePrefix[0] != '\0')
			sprintf_s(lpszCurDir, ("%s[%s]%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-syn.%s"), lpszCurDir, m_szFilePrefix,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, m_szFileExt);
		else
			sprintf_s(lpszCurDir, ("%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-syn.%s"), lpszCurDir,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, m_szFileExt);

		HANDLE hTemp = CreateFileA(lpszCurDir, GENERIC_WRITE,
								   FILE_SHARE_READ, NULL, CREATE_ALWAYS,
								   FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_OVERLAPPED*/, 0);
		if (GetFileSize(hTemp, NULL) == 0)
		{
			//Unicode文件的头2个字节标记
			WORD wFlag = 0xFEFF;
			DWORD dwIOSize = 0;
			WriteFile(hTemp, &wFlag, sizeof(WORD), &dwIOSize, NULL);
		}
		return hTemp;
	}

	HANDLE CLogManagerW::OpenAsynLogFile(const char* pstrFileName)
	{
		char lpszCurDir[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, lpszCurDir, MAX_PATH);
		char* pEnd = strrchr(lpszCurDir, '\\');
		if (pEnd)
			*pEnd = '\0';
		strcat_s(lpszCurDir, 256, "\\");
		strcat_s(lpszCurDir, 256, pstrFileName);
		strcat_s(lpszCurDir, 256, "\\");

		DWORD dwProcessID = GetCurrentProcessId();

		SYSTEMTIME time;
		ZeroMemory(&time, sizeof(SYSTEMTIME));
		GetLocalTime(&time);

		if (m_szFilePrefix[0] != '\0')
		{
			sprintf_s(lpszCurDir, 256, ("%s%d-%.2d\\%s\\"), lpszCurDir, time.wYear, time.wMonth, m_szFilePrefix);
		}
		else
		{
			sprintf_s(lpszCurDir, 256, ("%s%d-%.2d\\"), lpszCurDir, time.wYear, time.wMonth);
		}

		if (m_szFileExt[0] == '\0')
		{
			strncpy_s(m_szFileExt, "log", 15);
		}

		if (!MakeSureDirectoryPathExists(lpszCurDir))
		{
			return INVALID_HANDLE_VALUE;
		}
		if (m_szFilePrefix[0] != '\0')
			sprintf_s(lpszCurDir, ("%s%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-[%d]%s.%s"), lpszCurDir, m_szFilePrefix,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, dwProcessID, pstrFileName, m_szFileExt);
		else
			sprintf_s(lpszCurDir, ("%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-[%d]%s.%s"), lpszCurDir,
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, dwProcessID, pstrFileName, m_szFileExt);

		HANDLE hTemp = CreateFileA(lpszCurDir, GENERIC_WRITE,
								   FILE_SHARE_READ, NULL, CREATE_ALWAYS,
								   FILE_ATTRIBUTE_NORMAL/*|FILE_FLAG_OVERLAPPED*/, 0);
		if (GetFileSize(hTemp, NULL) == 0)
		{
			//Unicode文件的头2个字节标记
			WORD wFlag = 0xFEFF;
			DWORD dwIOSize = 0;
			WriteFile(hTemp, &wFlag, sizeof(WORD), &dwIOSize, NULL);
		}
		return hTemp;
	}

	void CLogManagerW::ShutDown()
	{
		LOG("LogManager ShutDown..");
		PostQueuedCompletionStatus(m_hIOCP, 0, 0, NULL);

		WaitForMultipleObjects(1, &m_hThread, TRUE, 10000);
		CloseHandle(m_hThread);
		m_hThread = INVALID_HANDLE_VALUE;
		CloseHandle(m_hIOCP);
		m_hIOCP = INVALID_HANDLE_VALUE;
		CloseHandle(m_hSynFileHandle);
		m_hSynFileHandle = INVALID_HANDLE_VALUE;
	}
#else
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CLogManager Linux

	CLogManager::CLogManager() :m_pool("CLogBufferA")
	{
		m_listLogBuffer.clear();

		m_semAsynLog.Create(NULL, 0, MAXINT);

		m_hThread = INVALID_HANDLE_VALUE;
		m_hAsynFileHandle = NULL;
		m_hSynFileHandle = NULL;
		m_bShutdown = FALSE;
		m_bInited = FALSE;
		ZeroMemory(m_szFilePrefix, sizeof(m_szFilePrefix));
		ZeroMemory(m_szFileExt, sizeof(m_szFileExt));

		pthread_create(&m_hThread, NULL, ThreadFunction, this);
	}

	CLogManager::~CLogManager()
	{
		if (m_hAsynFileHandle)
		{
			fclose(m_hAsynFileHandle);
		}

		if (m_hSynFileHandle)
		{
			fclose(m_hSynFileHandle);
		}
	}

	void* CLogManager::ThreadFunction(void* arg)
	{
		pthread_detach(pthread_self());

		BlockSignal();

		CLogManager* pLogManager = (CLogManager*) arg;

		pLogManager->ThreadWriteAsynLOG();

		return NULL;
	}

	void CLogManager::Init(LPCSTR pszPrefix)
	{
		if (m_bInited == FALSE)
		{
			if (pszPrefix && *pszPrefix != '\0')
			{
				strncpy_s(m_szFilePrefix, pszPrefix, FILE_PREFIX_MAXLEN - 1);
				m_szFilePrefix[FILE_PREFIX_MAXLEN - 1] = '\0';
			}
			else
			{
				m_szFilePrefix[0] = '\0';
			}

			m_hAsynFileHandle = OpenAsynLogFile(m_wzFilePath);
			m_hSynFileHandle = OpenSynLogFile();
			m_bInited = TRUE;
		}
	}

	BOOL CLogManager::PostBufferReady(CLogManager* pLogMgr, CLogBufferA* pBuffer, EM_IO_TYPE)
	{
		return pLogMgr->PostBufferReady(pBuffer);
	}

	void CLogManager::WriteSynLog(CLogManager* pLogMgr, CLogBufferA* pBuffer)
	{
		pLogMgr->LockSyncFile();

		cout << pBuffer->GetBuffer() << endl;

		DWORD dwIOSize = 0;

		FILE* pSyncFile = pLogMgr->GetSyncFileHandle();

		if (pSyncFile != NULL)
		{
			fwrite(pBuffer->GetBuffer(), pBuffer->GetSize(), 1, pSyncFile);
		}

		pLogMgr->UnlockSyncFile();
	}

	void CLogManager::ThreadWriteAsynLOG()
	{
		while (true)
		{
			m_semAsynLog.Lock();

			if (m_bShutdown)
			{
				break;
			}

			if (m_listLogBuffer.size() == 0)
			{
				continue;
			}

			m_lockAsynFile.Lock();

			CLogBufferA* pLogBuffer = m_listLogBuffer.front();
			m_listLogBuffer.pop_front();

			m_lockAsynFile.UnLock();

			if (pLogBuffer == NULL)
			{
				ReOpenAsynLogFile();
				continue;
			}

			if (pLogBuffer && m_hAsynFileHandle)
			{
				size_t nRealWrite = fwrite(pLogBuffer->GetBuffer(), pLogBuffer->GetSize(), 1, m_hAsynFileHandle);
				fflush(m_hAsynFileHandle);
			}

			if (pLogBuffer)
			{
				m_pool.ReclaimObj(pLogBuffer);
			}
		}
	}

	FILE* CLogManager::OpenAsynLogFile(const char* pstrFileName)
	{
		char lpszCurDir[MAX_PATH] = { 0 };
		readlink("/proc/self/exe", lpszCurDir, MAX_PATH);
		char* pEnd = strrchr(lpszCurDir, '/');
		if (pEnd)
			*pEnd = '\0';
		strcat_s(lpszCurDir, 256, "/");
		strcat_s(lpszCurDir, 256, pstrFileName);
		strcat_s(lpszCurDir, 256, "/");

		DWORD dwProcessID = getpid();

		tm timenow;
		time_t tCurTime = time(NULL);
		localtime_r(&tCurTime, &timenow);

		char szFullPath[MAX_PATH] = { 0 };

		if (m_szFilePrefix[0] != '\0')
		{
			sprintf_s(szFullPath, 256, ("%s%d-%.2d/%s/"), lpszCurDir, (timenow.tm_year + 1900), (timenow.tm_mon + 1), m_szFilePrefix);
		}
		else
		{
			sprintf_s(szFullPath, 256, ("%s%d-%.2d/"), lpszCurDir, (timenow.tm_year + 1900), (timenow.tm_mon + 1));
		}

		if (m_szFileExt[0] == '\0')
		{
			strncpy_s(m_szFileExt, "log", 15);
		}

		if (!MakeSureDirectoryPathExists(szFullPath))
		{
			return NULL;
		}

		char szFinalFileName[MAX_PATH] = { 0 };

		if (m_szFilePrefix[0] != '\0')
			sprintf_s(szFinalFileName, MAX_PATH, ("%s%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-[%d]%s.%s"), szFullPath, m_szFilePrefix,
			(timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, dwProcessID, pstrFileName, m_szFileExt);
		else
			sprintf_s(szFinalFileName, MAX_PATH, ("%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-[%d]%s.%s"), szFullPath,
			(timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, dwProcessID, pstrFileName, m_szFileExt);

		// printf("Cur Asyn LOG Path = [%s]\n", szFinalFileName);

		FILE* pFile = fopen(szFinalFileName, "w");

		if (pFile == NULL)
		{
			cerr << "LOG File [" << szFinalFileName << "]Create Failed : " << errno << endl;
		}

		return pFile;
	}

	void CLogManager::ReOpenAsynLogFile()
	{
		m_lockAsynFile.Lock();
		if (m_hAsynFileHandle)
		{
			fclose(m_hAsynFileHandle);
		}
		m_hAsynFileHandle = OpenAsynLogFile(m_wzFilePath);
		m_lockAsynFile.UnLock();
	}

	FILE* CLogManager::OpenSynLogFile()
	{
		char lpszCurDir[MAX_PATH] = { 0 };
		readlink("/proc/self/exe", lpszCurDir, MAX_PATH);
		char* pEnd = strrchr(lpszCurDir, '/');
		if (pEnd)
			*pEnd = '\0';
		strcat_s(lpszCurDir, 256, "/");
		strcat_s(lpszCurDir, 256, LOG_FILE_SYN);
		strcat_s(lpszCurDir, 256, "/");

		tm timenow;
		time_t tCurTime = time(NULL);
		localtime_r(&tCurTime, &timenow);

		char szFullPath[MAX_PATH] = { 0 };

		if (m_szFilePrefix[0] != '\0')
		{
			sprintf_s(szFullPath, 256, ("%s%d-%.2d/%s/"), lpszCurDir, (timenow.tm_year + 1900), (timenow.tm_mon + 1), m_szFilePrefix);
		}
		else
		{
			sprintf_s(szFullPath, 256, ("%s%d-%.2d/"), lpszCurDir, (timenow.tm_year + 1900), (timenow.tm_mon + 1));
		}

		if (m_szFileExt[0] == '\0')
		{
			strncpy_s(m_szFileExt, "log", 15);
		}

		if (!MakeSureDirectoryPathExists(szFullPath))
		{
			return NULL;
		}

		char szFinalFileName[MAX_PATH] = { 0 };

		if (m_szFilePrefix[0] != '\0')
			sprintf_s(szFinalFileName, MAX_PATH, ("%s[%s]%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-syn.%s"), szFullPath, m_szFilePrefix,
			(timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, m_szFileExt);
		else
			sprintf_s(szFinalFileName, MAX_PATH, ("%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-syn.%s"), szFullPath,
			(timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, m_szFileExt);

		FILE* pFile = fopen(szFinalFileName, "w");

		return pFile;
	}

	void CLogManager::ReOpenSynLogFile()
	{
		m_lockSynFile.Lock();

		if (m_hSynFileHandle)
		{
			fclose(m_hSynFileHandle);
		}

		m_hSynFileHandle = OpenSynLogFile();

		m_lockSynFile.UnLock();
	}

	BOOL CLogManager::PostBufferReady(CLogBufferA* pBuffer)
	{
		m_lockAsynFile.Lock();

		m_listLogBuffer.push_back(pBuffer);

		m_lockAsynFile.UnLock();

		m_semAsynLog.UnLock(1);
	}

	void CLogManager::ShutDown()
	{
		m_bShutdown = TRUE;
		m_semAsynLog.UnLock(1);
		if (m_hAsynFileHandle)
		{
			fclose(m_hAsynFileHandle);
			m_hAsynFileHandle = NULL;
		}
		if (m_hSynFileHandle)
		{
			fclose(m_hSynFileHandle);
			m_hSynFileHandle = NULL;
		}
	}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// CLogManagerW Linux

	CLogManagerW::CLogManagerW() :m_pool("CLogBufferW")
	{
		m_listLogBuffer.clear();

		m_semAsynLog.Create(NULL, 0, MAXINT);

		m_hThread = INVALID_HANDLE_VALUE;
		m_hAsynFileHandle = NULL;
		m_hSynFileHandle = NULL;
		m_bShutdown = FALSE;
		m_bInited = FALSE;
		ZeroMemory(m_szFilePrefix, sizeof(m_szFilePrefix));
		ZeroMemory(m_szFileExt, sizeof(m_szFileExt));

		pthread_create(&m_hThread, NULL, ThreadFunction, this);
	}

	CLogManagerW::~CLogManagerW()
	{
		if (m_hAsynFileHandle)
		{
			fclose(m_hAsynFileHandle);
		}

		if (m_hSynFileHandle)
		{
			fclose(m_hSynFileHandle);
		}
	}

	void CLogManagerW::Init(LPCSTR pszPrefix)
	{
		if (m_bInited == FALSE)
		{
			if (pszPrefix && *pszPrefix != '\0')
			{
				strncpy_s(m_szFilePrefix, pszPrefix, FILE_PREFIX_MAXLEN - 1);
				m_szFilePrefix[FILE_PREFIX_MAXLEN - 1] = '\0';
			}
			else
			{
				m_szFilePrefix[0] = '\0';
			}

			m_hAsynFileHandle = OpenAsynLogFile(m_wzFilePath);
			m_hSynFileHandle = OpenSynLogFile();
			m_bInited = TRUE;
		}
	}

	void CLogManagerW::WriteSynLog(CLogManagerW* pLogMgr, CLogBufferW* pBuffer)
	{
		pLogMgr->LockSyncFile();

		wprintf(pBuffer->GetBuffer());

		DWORD dwIOSize = 0;

		FILE* pSyncFile = pLogMgr->GetSyncFileHandle();

		if (pSyncFile != NULL)
		{
			fwrite(pBuffer->GetBuffer(), (pBuffer->GetSize()*sizeof(wchar_t)), 1, pSyncFile);
		}

		pLogMgr->UnlockSyncFile();
	}

	BOOL CLogManagerW::PostBufferReady(CLogManagerW* pLogMgr, CLogBufferW* pBuffer, EM_IO_TYPE)
	{
		return pLogMgr->PostBufferReady(pBuffer);
	}

	void* CLogManagerW::ThreadFunction(void* arg)
	{
		pthread_detach(pthread_self());

		BlockSignal();

		CLogManagerW* pLogManager = (CLogManagerW*) arg;

		pLogManager->ThreadWriteAsynLOG();

		return NULL;
	}

	void CLogManagerW::ThreadWriteAsynLOG()
	{
		while (true)
		{
			m_semAsynLog.Lock();

			if (m_bShutdown)
			{
				break;
			}

			if (m_listLogBuffer.size() == 0)
			{
				continue;
			}

			m_lockAsynFile.Lock();

			CLogBufferW* pLogBuffer = m_listLogBuffer.front();
			m_listLogBuffer.pop_front();

			m_lockAsynFile.UnLock();

			if (pLogBuffer == NULL)
			{
				ReOpenAsynLogFile();
				continue;
			}

			if (pLogBuffer && m_hAsynFileHandle)
			{
				char utf8_buffer[1024 X16] = { 0 };
				unicodetoutf8(pLogBuffer->GetBuffer(), utf8_buffer, 1024 X16);
				size_t nRealWrite = fwrite(utf8_buffer, strlen(utf8_buffer), 1, m_hAsynFileHandle);
				fflush(m_hAsynFileHandle);
			}
			if (pLogBuffer)
			{
				m_pool.ReclaimObj(pLogBuffer);
			}
		}
	}

	void CLogManagerW::ShutDown()
	{
		m_bShutdown = TRUE;
		m_semAsynLog.UnLock(1);
		if (m_hAsynFileHandle)
		{
			fclose(m_hAsynFileHandle);
			m_hAsynFileHandle = NULL;
		}
		if (m_hSynFileHandle)
		{
			fclose(m_hSynFileHandle);
			m_hSynFileHandle = NULL;
		}
	}

	FILE* CLogManagerW::OpenAsynLogFile(const char* pstrFileName)
	{
		char lpszCurDir[MAX_PATH] = { 0 };
		readlink("/proc/self/exe", lpszCurDir, MAX_PATH);
		char* pEnd = strrchr(lpszCurDir, '/');
		if (pEnd)
			*pEnd = '\0';
		strcat_s(lpszCurDir, 256, "/");
		strcat_s(lpszCurDir, 256, pstrFileName);
		strcat_s(lpszCurDir, 256, "/");

		DWORD dwProcessID = getpid();

		tm timenow;
		time_t tCurTime = time(NULL);
		localtime_r(&tCurTime, &timenow);

		char szFullPath[MAX_PATH] = { 0 };

		if (m_szFilePrefix[0] != '\0')
		{
			sprintf_s(szFullPath, 256, ("%s%d-%.2d/%s/"), lpszCurDir, (timenow.tm_year + 1900), (timenow.tm_mon + 1), m_szFilePrefix);
		}
		else
		{
			sprintf_s(szFullPath, 256, ("%s%d-%.2d/"), lpszCurDir, (timenow.tm_year + 1900), (timenow.tm_mon + 1));
		}

		if (m_szFileExt[0] == '\0')
		{
			strncpy_s(m_szFileExt, "log", 15);
		}

		if (!MakeSureDirectoryPathExists(szFullPath))
		{
			return NULL;
		}

		char szFinalFileName[MAX_PATH] = { 0 };

		if (m_szFilePrefix[0] != '\0')
			sprintf_s(szFinalFileName, MAX_PATH, ("%s%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-[%d]%s.%s"), szFullPath, m_szFilePrefix,
			(timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, dwProcessID, pstrFileName, m_szFileExt);
		else
			sprintf_s(szFinalFileName, MAX_PATH, ("%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-[%d]%s.%s"), szFullPath,
			(timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, dwProcessID, pstrFileName, m_szFileExt);

		// printf("Cur Asyn LOG Path = [%s]\n", szFinalFileName);

		FILE* pFile = fopen(szFinalFileName, "w");

		if (pFile == NULL)
		{
			cerr << "LOG File [" << szFinalFileName << "]Create Failed : " << errno << endl;
		}

		// 	if ( GetFileSize(pFile) == 0 )
		// 	{
		// 		WORD wFlag = 0xFEFF;
		// 		fwrite((&wFlag),sizeof(WORD),1,pFile);
		// 	}

		return pFile;
	}

	void CLogManagerW::ReOpenAsynLogFile()
	{
		m_lockAsynFile.Lock();
		if (m_hAsynFileHandle)
		{
			fclose(m_hAsynFileHandle);
		}
		m_hAsynFileHandle = OpenAsynLogFile(m_wzFilePath);
		m_lockAsynFile.UnLock();
	}

	FILE* CLogManagerW::OpenSynLogFile()
	{
		char lpszCurDir[MAX_PATH] = { 0 };
		readlink("/proc/self/exe", lpszCurDir, MAX_PATH);
		char* pEnd = strrchr(lpszCurDir, '/');
		if (pEnd)
			*pEnd = '\0';
		strcat_s(lpszCurDir, 256, "/");
		strcat_s(lpszCurDir, 256, LOG_FILE_SYN);
		strcat_s(lpszCurDir, 256, "/");

		tm timenow;
		time_t tCurTime = time(NULL);
		localtime_r(&tCurTime, &timenow);

		char szFullPath[MAX_PATH] = { 0 };

		if (m_szFilePrefix[0] != '\0')
		{
			sprintf_s(szFullPath, 256, ("%s%d-%.2d/%s/"), lpszCurDir, (timenow.tm_year + 1900), (timenow.tm_mon + 1), m_szFilePrefix);
		}
		else
		{
			sprintf_s(szFullPath, 256, ("%s%d-%.2d/"), lpszCurDir, (timenow.tm_year + 1900), (timenow.tm_mon + 1));
		}

		if (m_szFileExt[0] == '\0')
		{
			strncpy_s(m_szFileExt, "log", 15);
		}

		if (!MakeSureDirectoryPathExists(lpszCurDir))
		{
			return NULL;
		}

		char szFinalFileName[MAX_PATH] = { 0 };

		if (m_szFilePrefix[0] != '\0')
			sprintf_s(szFinalFileName, MAX_PATH, ("%s[%s]%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-syn.%s"), szFullPath, m_szFilePrefix,
			(timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, m_szFileExt);
		else
			sprintf_s(szFinalFileName, MAX_PATH, ("%s%.2d-%.2d-%.2d-%.2d-%.2d-%.2d-syn.%s"), szFullPath,
			(timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, m_szFileExt);

		FILE* pFile = fopen(szFinalFileName, "w");

		if (pFile == NULL)
		{
			cerr << "LOG File [" << szFinalFileName << "]Create Failed : " << errno << endl;
		}

		// 	if (GetFileSize(pFile) == 0)
		// 	{
		// 		WORD wFlag = 0xFEFF;
		// 		fwrite((&wFlag), sizeof(WORD), 1, pFile);
		// 	}

		return pFile;
	}

	void CLogManagerW::ReOpenSynLogFile()
	{
		m_lockSynFile.Lock();

		fclose(m_hSynFileHandle);

		m_hSynFileHandle = OpenSynLogFile();

		m_lockSynFile.UnLock();
	}

	BOOL CLogManagerW::PostBufferReady(CLogBufferW* pBuffer)
	{
		m_lockAsynFile.Lock();

		m_listLogBuffer.push_back(pBuffer);

		m_lockAsynFile.UnLock();

		m_semAsynLog.UnLock(1);
	}

#endif // LIB_WINDOWS

}

