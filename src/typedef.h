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

#ifndef __TYPEDEF_H_
#define __TYPEDEF_H_

#ifdef LIB_LINUX
typedef unsigned int UINT;

typedef unsigned long long ULONGLONG;

typedef long long LONGLONG;

typedef long long __int64;

typedef int BOOL;

#define TRUE		1
#define FALSE		0

typedef int HANDLE;
#define INVALID_HANDLE_VALUE ((HANDLE)-1)

typedef int INT,*PINT;

typedef int SOCKET;

typedef unsigned int *PUINT;

typedef long LONG_PTR, *PLONG_PTR;

typedef int INT_PTR, *PINT_PTR;

typedef LONG_PTR SSIZE_T, *PSSIZE_T;

typedef unsigned char BYTE;

typedef BYTE *LPBYTE;

typedef char CHAR;

typedef long LONG;

typedef wchar_t WCHAR;

typedef const char* LPCSTR;

typedef char* LPSTR;

typedef const wchar_t* LPCWSTR;

typedef wchar_t* LPWSTR;

typedef unsigned short WORD;

typedef unsigned long DWORD;

typedef DWORD ULONG;

typedef short SHORT;

typedef unsigned short USHORT;

typedef unsigned long long UINT_PTR, *PUINT_PTR;

typedef unsigned long long UINT64;

typedef long long INT64;

typedef double DOUBLE;

typedef void* LPVOID;

typedef time_t __time32_t;

typedef LONGLONG __time64_t;

#define _TRUNCATE ((size_t)-1)

#ifndef interface
#define interface struct
#endif

#define _countof(_Array) (sizeof(_Array) / sizeof(_Array[0]))

#define MAXCHAR			(0x7f)
#define MAXSHORT		(0x7fff)
#define MAXLONG			(0x7fffffff)
#define MAXLONGLONG		(0x7fffffffffffffff)

#define MAXINT ((INT)(MAXUINT >> 1))
#define MAXUINT ((UINT)~((UINT)0))
#define MAXULONG32 ((ULONG32)~((ULONG32)0))

#define INFINITE		MAXLONG

#define MAX_PATH 260

#define WINAPI

#define strcpy_s		strcpy
#define strncpy_s		strncpy

#define wcscpy_s		wcscpy
#define wcsncpy_s		wcsncpy

#define _snprintf_s		snprintf
#define sprintf_s		snprintf
#define _vsnprintf_s	vsnprintf
#define vsnprintf_s		vsnprintf

#define _snwprintf_s	swprintf
#define swprintf_s		swprintf
#define _vsnwprintf_s	vswprintf

#define StrCmpIW wcscasecmp
#define StrCmpIA strcasecmp

#define _wcsicmp wcscasecmp
#define _stricmp strcasecmp

#define StrStrIA strcasestr

#define __snprintf_s(_STR,_LEN,_FMT,args...) snprintf(_STR,_LEN,_FMT,##args)
#define __vsnprintf_s(_STR,_LEN,_FMT,_VAR) vsnprintf(_STR,_LEN,_FMT,_VAR)

#define __snwprintf_s(_STR,_LEN,_FMT,args...) swprintf(_STR,_LEN,_FMT,##args)
#define __vsnwprintf_s(_STR,_LEN,_FMT,_VAR) vswprintf(_STR,_LEN,_FMT,_VAR)

#define __strncpy_s(_DST,_SRC,_LEN) strncpy(_DST,_SRC,_LEN)

#define __wcsncpy_s(_DST,_SRC,_LEN) wcsncpy(_DST,_SRC,_LEN)

#define gmtime_s(_tm,_ts)		gmtime_r(_ts,_tm)

#define _mktime32		mktime

#define ExitProcess		exit

#define ZeroMemory(_Dst,_Size) memset((_Dst),0,(_Size))

#define printf_s		printf
#define fprintf_s		fprintf

#define GetLastError() errno
#define WSAGetLastError() errno

#define vsprintf_s vsnprintf

#define strcat_s(_Dst,_DstMax,_Src) strncat(_Dst,_Src,_DstMax)

#define Sleep(t) usleep((t*1000))

#define _ASSERT(_expr) assert(_expr)
#define _ASSERTE(_expr) assert(_expr)

#define SOCKET_ERROR (-1)

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif // !INVALID_SOCKET

#define closesocket close

#ifdef NULL
#undef NULL
#endif

#define NULL nullptr

DWORD GetTickCount();

int _wtoi(const wchar_t* pwzSrc);

long _wtol(const wchar_t* pwzSrc);

double _wtof(const wchar_t* pwzSrc);

LONGLONG _wtoll(const wchar_t* pwzSrc);

LONGLONG _wtoi64(const wchar_t *pwzSrc);

LONGLONG _atoi64(const char* _String);

void SetConsoleTitleA(const char* pszTitle);

typedef struct _SYSTEMTIME {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

void GetLocalTime(LPSYSTEMTIME lpSystemTime);

#define localtime32(_TV,_TM) localtime_r(_TV,_TM)

#else
#define __snprintf_s(_STR,_LEN,_FMT,...) _snprintf_s(_STR,_LEN,_LEN,_FMT,__VA_ARGS__)
#define __vsnprintf_s(_STR,_LEN,_FMT,_VAR) vsnprintf_s(_STR,_LEN,_LEN,_FMT,_VAR)

#define __snwprintf_s(_STR,_LEN,_FMT,...) _snwprintf_s(_STR,_LEN,_LEN,_FMT,__VA_ARGS__)
#define __vsnwprintf_s(_STR,_LEN,_FMT,_VAR) vswprintf_s(_STR,_LEN,_LEN,_FMT,_VAR)

#define __strncpy_s(_DST,_SRC,_LEN) strncpy_s(_DST,_LEN,_SRC,_LEN)

#define __wcsncpy_s(_DST,_SRC,_LEN) wcsncpy_s(_DST,_LEN,_SRC,_LEN)

typedef unsigned int UINT;

typedef int INT;

typedef double DOUBLE;

typedef __int64 LONGLONG;

typedef const char* LPCSTR;

typedef const wchar_t* LPCWSTR;

#ifdef NULL
#undef NULL
#endif

#define NULL nullptr

#define localtime32(_TV,_TM) _localtime32_s(_TM,_TV)

#endif // #ifdef LIB_LINUX

#endif