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
#include "Utility.h"
#include <time.h>

#ifdef LIB_WINDOWS
#include "../xcpthlr/ExceptHandler.h"
#else
#include <sys/sysinfo.h>
#endif // LIB_WINDOWS

void StrDelimiter(const char *lpszSrc, const char *lpszDelimiter, std::vector<std::string> &vecResult)
{
	std::string strSrc = lpszSrc;

	StrDelimiter(strSrc, lpszDelimiter, vecResult);
}
void StrDelimiter( const wchar_t* lpwzSrc, const wchar_t* lpwzDelimiter, std::vector<std::wstring> &vctResult )
{	//注意：会修改原字符串数据
	std::wstring wstrSrc = lpwzSrc;

	StrDelimiter(wstrSrc,lpwzDelimiter,vctResult);
}
void StrDelimiter( std::wstring& wstrSrc, const wchar_t* lpwzDelimiter, std::vector<std::wstring> &vctResult )
{
	if (wstrSrc.size() == 0)
	{
		return;
	}

	size_t nDelimiterSize = wcslen(lpwzDelimiter);
	size_t nPos = wstrSrc.find(lpwzDelimiter);
	size_t nPosPrev = 0;

	while (nPos != wstrSrc.npos)
	{
		std::wstring wstrToken = wstrSrc.substr(nPosPrev,nPos-nPosPrev);

		if (wstrToken.size() > 0)
		{
			vctResult.push_back(wstrToken);
		}

		nPosPrev = nPos + nDelimiterSize;
		nPos = wstrSrc.find(lpwzDelimiter,nPosPrev);
	}

	if (wstrSrc.size() != nPosPrev)
	{
		std::wstring wstrToken = wstrSrc.substr(nPosPrev,wstrSrc.size()-nPosPrev);

		vctResult.push_back(wstrToken);
	}
}

void StrDelimiter( std::wstring& wstrSrc, const wchar_t* lpwzDelimiter, std::vector<int> &vctResult)
{
	std::vector<std::wstring> vctResultW;

	StrDelimiter(wstrSrc, lpwzDelimiter, vctResultW);

	for (size_t i = 0; i < vctResultW.size(); ++i)
	{
		int nValue = _wtoi(vctResultW[i].c_str());

		vctResult.push_back(nValue);
	}
}

void StrDelimiter(const wchar_t* lpwzSrc, const wchar_t* lpwzDelimiter, std::vector<int> &vctResult)
{
	std::vector<std::wstring> vctResultW;

	StrDelimiter(lpwzSrc,lpwzDelimiter,vctResultW);

	for (size_t i = 0;i < vctResultW.size();++i)
	{
		int nValue = _wtoi(vctResultW[i].c_str());

		vctResult.push_back(nValue);
	}
}

void StrDelimiter( std::string& strSrc, const char* lpzDelimiter, std::vector<std::string> &vctResult )
{
	if (strSrc.size() == 0)
	{
		return;
	}

	size_t nDelimiterSize = strlen(lpzDelimiter);
	size_t nPos = strSrc.find(lpzDelimiter);
	size_t nPosPrev = 0;

	while (nPos != strSrc.npos)
	{
		std::string strToken = strSrc.substr(nPosPrev,nPos-nPosPrev);

		if (strToken.size() > 0)
		{
			vctResult.push_back(strToken);
		}

		nPosPrev = nPos + nDelimiterSize;
		nPos = strSrc.find(lpzDelimiter,nPosPrev);
	}

	if (strSrc.size() != nPosPrev)
	{
		std::string strToken = strSrc.substr(nPosPrev,strSrc.size()-nPosPrev);

		vctResult.push_back(strToken);
	}
}

void StrDelimiter(const char *lpszSrc, const char *lpszDelimiter, std::vector<int> &vctResult)
{
	vctResult.clear();

	std::vector<std::string> vctResultA;

	StrDelimiter(lpszSrc, lpszDelimiter, vctResultA);

	for (size_t i = 0; i < vctResultA.size(); ++i)
	{
		int nValue = atoi(vctResultA[i].c_str());

		vctResult.push_back(nValue);
	}
}

void StrDelimiter(std::string& strSrc, const char *lpszDelimiter, std::vector<int> &vctResult)
{
	vctResult.clear();

	std::vector<std::string> vctResultA;

	StrDelimiter(strSrc, lpszDelimiter, vctResultA);

	for (size_t i = 0; i < vctResultA.size(); ++i)
	{
		int nValue = atoi(vctResultA[i].c_str());

		vctResult.push_back(nValue);
	}
}

void FillSpacing(char* pBuffer, char cDelimiter, char cFill)
{
    if (cDelimiter == cFill)
        return;
    size_t nLen = strlen(pBuffer);
    for (size_t i=0; i<nLen; ++i)
    {
        if (pBuffer[i]==cDelimiter && pBuffer[i+1]==cDelimiter)
        {
            memmove(pBuffer+i+2, pBuffer+i+1, nLen-i-1);
            pBuffer[i+1] = cFill;
            ++nLen;
        }
    }
}

bool IsDirExist(char *lpszDir)
{
#ifdef LIB_WINDOWS
	DWORD dwRet = GetFileAttributesA(lpszDir);

	if (!((dwRet != 0xFFFFFFFF) && (dwRet & FILE_ATTRIBUTE_DIRECTORY)))
		return false;
	else
		return true;
#endif // LIB_WINDOWS

#ifdef LIB_LINUX
	if(access(lpszDir,F_OK) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
#endif // LIB_LINUX
}

std::mt19937& RandomHelper::getEngine()
{
	static std::random_device seed_gen;
	static std::mt19937 engine(seed_gen());
	return engine;
}

int Random2( int nMax, int nMin )
{
	if (nMin == nMax)
		return nMax;
	if (nMax < nMin)
		return Random2(nMin, nMax);

	return RandomHelper::random_int(nMin, nMax-1);
}

UINT Random2(UINT nMax, UINT nMin /*= 0*/)
{
	if (nMin == nMax)
		return nMax;
	if (nMax < nMin)
		return Random2(nMin, nMax);

	return RandomHelper::random_int(nMin, nMax - 1);
}

long Random2(long nMax, long nMin)
{
	if (nMin == nMax)
		return nMax;
	if (nMax < nMin)
		return Random2(nMin, nMax);

	return RandomHelper::random_int(nMin, nMax-1);
}

LONGLONG Random2(LONGLONG nMax, LONGLONG nMin)
{
	if (nMin == nMax)
		return nMax;
	if (nMax < nMin)
		return Random2(nMin, nMax);

	return RandomHelper::random_int(nMin, nMax-1);
}

DOUBLE Random2(DOUBLE dMax, DOUBLE dMin /*= 0.000000*/)
{
	if (abs(dMin-dMax) < 0.000001)
		return dMax;
	if (dMax < dMin)
		return Random2(dMin, dMax);

	return RandomHelper::random_real(dMin, dMax);
}

char* Trim(char* lpszStr)
{
    TrimL(lpszStr);
    TrimR(lpszStr);

    return lpszStr;
}
wchar_t* TrimW(wchar_t* lpswStr )
{
	TrimLW( lpswStr );
	TrimRW( lpswStr );
	return lpswStr;
}
char* TrimL(char *lpszStr)
{
    if(lpszStr == NULL)	
    {
        return NULL;
    }

    int nLen = (int)strlen(lpszStr);
    if(nLen == 0)
    {
        return lpszStr;
    }

    int i = 0;
    //while( i<nLen && isspace( *(str+i) ) )
    while( i<nLen && (*(lpszStr+i) == '\r' || *(lpszStr+i) == '\n' || *(lpszStr+i) == '\t' || *(lpszStr+i) == ' ') )
    {
        i++;
    }

	__strncpy_s(lpszStr, (lpszStr + i), nLen);

    return lpszStr;
}
wchar_t* TrimLW(wchar_t* lpswStr )
{
	if(lpswStr == NULL)	
	{
		return NULL;
	}

	int nLen = (int)wcslen(lpswStr);
	if(nLen == 0)
	{
		return lpswStr;
	}

	int i = 0;
	//while( i<nLen && isspace( *(str+i) ) )
	while( i<nLen && (*(lpswStr+i) == L'\r' || *(lpswStr+i) == L'\n' || *(lpswStr+i) == L'\t' || *(lpswStr+i) == L' ') )
	{
		i++;
	}
	if( i == 0 )
		return lpswStr;

	__wcsncpy_s(lpswStr, (lpswStr + i), nLen);

	return lpswStr;
}
char* TrimR(char *lpszStr)
{
    if(lpszStr == NULL)
    {
        return NULL;
    }

    int nLen = (int)strlen(lpszStr);
    if(nLen == 0)
    {
        return lpszStr;
    }

    int i = nLen-1;
    //while(i>0 && isspace(*(str+i)))
    while(i>0 && (*(lpszStr+i) == '\r' || *(lpszStr+i) == '\n' || *(lpszStr+i) == '\t' || *(lpszStr+i) == ' '))
    {
        i--;
    }
    *(lpszStr+i+1) = 0;

    return lpszStr;
}
wchar_t* TrimRW(wchar_t* lpswStr)
{
	if(lpswStr == NULL)
	{
		return NULL;
	}

	int nLen = (int)wcslen(lpswStr);
	if(nLen == 0)
	{
		return lpswStr;
	}

	int i = nLen-1;
	//while(i>0 && isspace(*(str+i)))
	while(i>0 && (*(lpswStr+i) == L'\r' || *(lpswStr+i) == L'\n' || *(lpswStr+i) == L'\t' || *(lpswStr+i) == L' '))
	{
		i--;
	}
	*(lpswStr+i+1) = 0;

	return lpswStr;
}
char* GetDateTime(char *lpszTimeBuf, int nLen, char Flag)
{
    if(lpszTimeBuf == NULL) return NULL;

#ifdef LIB_WINDOWS
	SYSTEMTIME time;
	ZeroMemory(&time, sizeof(SYSTEMTIME));
	GetLocalTime(&time);

	switch (Flag)
	{
	case 'A'://2003-10-20 11:20:35
		sprintf_s(lpszTimeBuf, nLen, "%d-%.2d-%.2d %.2d:%.2d:%.2d_%.4d",
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		break;
	case 'S'://10-20 11:20:35
		sprintf_s(lpszTimeBuf, nLen, "%.2d-%.2d %.2d:%.2d:%.2d_%.4d",
				  time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		break;
	case 'D'://2003-10-20
		sprintf_s(lpszTimeBuf, nLen, "%d-%.2d-%.2d", time.wYear, time.wMonth, time.wDay);
		break;
	case 'T'://11:20:35
		sprintf_s(lpszTimeBuf, nLen, "%.2d:%.2d:%.2d_%.4d", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
		break;
	case 'W'://2003-10
		sprintf_s(lpszTimeBuf, nLen, "%d-%.2d", time.wYear, time.wMonth);
		break;
	default: //2003-10-20 11:20:35
		sprintf_s(lpszTimeBuf, nLen, "%d-%.2d-%.2d %.2d:%.2d:%.2d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
		break;
	}
#endif // LIB_WINDOWS

#ifdef LIB_LINUX
	time_t tCurTime = 0;
	time(&tCurTime);
	tm timenow;
	ZeroMemory(&timenow,sizeof(timenow));

	localtime_r(&tCurTime,&timenow);

	timeval tv;
	gettimeofday(&tv,NULL);

	switch (Flag)
	{
	case 'A'://2003-10-20 11:20:35_0520
		sprintf_s(lpszTimeBuf, nLen, "%d-%.2d-%.2d %.2d:%.2d:%.2d_%.6d",
				  (timenow.tm_year+1900),(timenow.tm_mon+1),timenow.tm_mday,timenow.tm_hour,timenow.tm_min,timenow.tm_sec,tv.tv_usec);
		break;
	case 'S'://10-20 11:20:35_0520
		sprintf_s(lpszTimeBuf, nLen, "%.2d-%.2d %.2d:%.2d:%.2d_%.6d",
				 (timenow.tm_mon+1),timenow.tm_mday,timenow.tm_hour,timenow.tm_min,timenow.tm_sec,tv.tv_usec);
		break;
	case 'D'://2003-10-20
		sprintf_s(lpszTimeBuf, nLen, "%d-%.2d-%.2d", (timenow.tm_year+1900),(timenow.tm_mon+1),timenow.tm_mday);
		break;
	case 'T'://11:20:35
		sprintf_s(lpszTimeBuf, nLen, "%.2d:%.2d:%.2d_%.6d", timenow.tm_hour,timenow.tm_min,timenow.tm_sec,tv.tv_usec);
		break;
	case 'W'://2003-10
		sprintf_s(lpszTimeBuf, nLen, "%d-%.2d", (timenow.tm_year+1900),(timenow.tm_mon+1));
		break;
	default: //2003-10-20 11:20:35
		sprintf_s(lpszTimeBuf, nLen, "%d-%.2d-%.2d %.2d:%.2d:%.2d",
				  (timenow.tm_year+1900),(timenow.tm_mon+1),timenow.tm_mday,timenow.tm_hour,timenow.tm_min,timenow.tm_sec);
		break;
	}
#endif // LIB_LINUX

    return lpszTimeBuf;
}

wchar_t* GetDateTime(wchar_t *lpwzTimeBuf, int nLen, char Flag )
{
	if(lpwzTimeBuf == NULL) return NULL;
#ifdef LIB_WINDOWS
	SYSTEMTIME time;
	ZeroMemory(&time,sizeof(SYSTEMTIME));
	GetLocalTime(&time);

	switch(Flag)
	{
	case 'A'://2003-10-20 11:20:35
		_snwprintf_s(lpwzTimeBuf, nLen,nLen, L"%d-%.2d-%.2d %.2d:%.2d:%.2d_%.4d",
						time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond, time.wMilliseconds);
		break;
	case 'S'://10-20 11:20:35
		_snwprintf_s(lpwzTimeBuf, nLen,nLen, L"%.2d-%.2d %.2d:%.2d:%.2d_%.4d",
					 time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond, time.wMilliseconds);
		break;
	case 'D'://2003-10-20
		_snwprintf_s(lpwzTimeBuf, nLen,nLen, L"%d-%.2d-%.2d",time.wYear,time.wMonth,time.wDay);
		break;
	case 'T'://11:20:35
		_snwprintf_s(lpwzTimeBuf, nLen,nLen, L"%.2d:%.2d:%.2d_%.4d",time.wHour,time.wMinute,time.wSecond, time.wMilliseconds);
		break;
	case 'W'://2003-10
		_snwprintf_s(lpwzTimeBuf, nLen,nLen, L"%d-%.2d",time.wYear,time.wMonth);
		break;
	default: //2003-10-20 11:20:35
		_snwprintf_s(lpwzTimeBuf, nLen,nLen, L"%d-%.2d-%.2d %.2d:%.2d:%.2d",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
		break;
	}
#endif

#ifdef LIB_LINUX
	time_t tCurTime = 0;
	time(&tCurTime);
	tm timenow;
	ZeroMemory(&timenow, sizeof(timenow));

	localtime_r(&tCurTime, &timenow);

	timeval tv;
	gettimeofday(&tv, NULL);

	switch (Flag)
	{
		case 'A'://2003-10-20 11:20:35_0520
			_snwprintf_s(lpwzTimeBuf, nLen, L"%d-%.2d-%.2d %.2d:%.2d:%.2d_%.6d",
					  (timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, tv.tv_usec);
			break;
		case 'S'://2003-10-20 11:20:35_0520
			_snwprintf_s(lpwzTimeBuf, nLen, L"%.2d-%.2d %.2d:%.2d:%.2d_%.6d",
						 (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec, tv.tv_usec);
			break;
		case 'D'://2003-10-20
			_snwprintf_s(lpwzTimeBuf, nLen, L"%d-%.2d-%.2d", (timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday);
			break;
		case 'T'://11:20:35
			_snwprintf_s(lpwzTimeBuf, nLen, L"%.2d:%.2d:%.2d_%.6d", timenow.tm_hour, timenow.tm_min, timenow.tm_sec, tv.tv_usec);
			break;
		case 'W'://2003-10
			_snwprintf_s(lpwzTimeBuf, nLen, L"%d-%.2d", (timenow.tm_year + 1900), (timenow.tm_mon + 1));
			break;
		default: //2003-10-20 11:20:35
			_snwprintf_s(lpwzTimeBuf, nLen, L"%d-%.2d-%.2d %.2d:%.2d:%.2d",
						 (timenow.tm_year + 1900), (timenow.tm_mon + 1), timenow.tm_mday, timenow.tm_hour, timenow.tm_min, timenow.tm_sec);
			break;
	}
#endif
	return lpwzTimeBuf;
}
char *DelSpace(char *s)
{
    char *p=s,*q=s;
    for(;*s==' ';s++);

    for(;*s;)
    {
        *q++=*s;
        if(*s!=' ')
            s++;
        else
            while(*s==' ') s++;
    }

    if( (q>p) && (*(q-1)==' ') )
        *(q-1)='\0';
    else
        *q='\0';

    return p;
}

std::string HexDump(const BYTE* pData, int len, const char *delim)
{
    static BYTE hextable[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string sResult, tmp;
    BYTE result;
    for (int i = 0; i < len; i++)
    {
        result = BYTE(pData[i]);
        sResult += hextable[(result >> 4) & 0x0F];
        sResult += hextable[result & 0x0F];
        sResult += delim;
    }
    return sResult;
}

void StrReplace(char *szSource, int nLen,const char *pszOldstring,const char *pszNewstring)
{
	std::string strSource = szSource;

	StrReplace(strSource,pszOldstring,pszNewstring);

#ifdef LIB_WINDOWS
	strcpy_s(szSource, nLen, strSource.c_str());
#else
	strncpy(szSource, strSource.c_str(),nLen);
#endif // LIB_WINDOWS
}

void StrReplace(wchar_t *wzSource, int nLen,const wchar_t *pwzOldstring,const wchar_t *pwzNewstring)
{
	std::wstring strSource = wzSource;

	StrReplace(strSource,pwzOldstring,pwzNewstring);

	__wcsncpy_s(wzSource, strSource.c_str(), nLen-1);
}


void StrReplace(std::string&strSource,const std::string&strOldstring,const std::string&strNewstring)
{
	if (strOldstring == strNewstring)
	{
		return;
	}

	std::string::size_type pos=0;
	std::string::size_type nOldSize=strOldstring.size();
	std::string::size_type nNewSize=strNewstring.size();

	if (nOldSize == 0)
	{
		return;
	}

	while((pos=strSource.find(strOldstring,pos))!=std::string::npos)
	{
		strSource.replace(pos,nOldSize,strNewstring);
		pos+=nNewSize;
	}
}

void StrReplace(std::wstring&strSource,const std::wstring&strOldstring,const std::wstring&strNewstring)
{
	if (strOldstring == strNewstring)
	{
		return;
	}
	std::wstring::size_type pos=0;
	std::wstring::size_type nOldSize=strOldstring.size();
	std::wstring::size_type nNewSize=strNewstring.size();

	if (nOldSize == 0)
	{
		return;
	}

	while((pos=strSource.find(strOldstring,pos))!=std::wstring::npos)
	{
		strSource.replace(pos,nOldSize,strNewstring);
		pos+=nNewSize;
	}
}

std::string vStrFormat(const char* format, va_list va)
{
	if (format == NULL || va == NULL)
	{
		return std::string("");
	}

	int i = 0;

	std::string str = format;

	do
	{
		char key[21] = { 0 };
		_snprintf_s(key, 20, "{%d}", i);

		if (str.find(key) == std::string::npos)
		{
			break;
		}

		std::string::size_type pos = 0;
		std::string::size_type oldsize = strlen(key);
		const char* newcstr = va_arg(va, const char*);
		if (newcstr == NULL)
		{
			break;
		}
		std::string newstr = newcstr;
		std::string::size_type newsize = newstr.size();

		while ((pos = str.find(key, pos)) != std::wstring::npos)
		{
			str.replace(pos, oldsize, newstr);
			pos += newsize;
		}

		++i;
	} while (true);

	return str;
}

std::string StrFormat(const char* format, ...)
{
	va_list va;

	va_start(va, format);

	std::string str = vStrFormat(format, va);

	va_end(va);

	return str;
}

//判断一天是不是当月的第一天
bool IsFirstDayOfMonth( const time_t& tTime )
{
    tm NewTime;

#ifdef LIB_WINDOWS
	if( localtime_s( &NewTime,&tTime ) == 0 && NewTime.tm_mday == 1 )
	{
		return true;
	}
#else
	if (localtime_r( &tTime,&NewTime ) == 0 && NewTime.tm_mday == 1)
	{
		return true;
	}
#endif // LIB_WINDOWS

    return false;
}

//判断一天是不是当月的最后一天
bool IsLastDayOfMonth( const time_t& tTime )
{
    tm NewTime;

#ifdef LIB_WINDOWS
	if (localtime_s(&NewTime, &tTime) != 0)
	{
		return false;
	}
#else
	if (localtime_r(&tTime, &NewTime) != 0)
	{
		return false;
	}
#endif

    if( NewTime.tm_mday <28 )
        return false;

    if( NewTime.tm_mday <30 && NewTime.tm_mon != 1 )
        return false;

    switch( NewTime.tm_mon )
    {
    case 1://2月
        {
            DWORD nYear = NewTime.tm_year;
            //闰年
            if( ((nYear % 4 == 0) && (nYear % 100 != 0)) && ( nYear % 400 == 0 ) )
            {
                if( NewTime.tm_mday == 29 )
                {
                    return true;
                }
            }
            //非闰年
            else
            {
                if( NewTime.tm_mday == 28 )
                {
                    return true;
                }
            }
        }
        break;

    case 0://1月
    case 2://3月
    case 4://5月
    case 6://7月
    case 7://8月
    case 9://10月
    case 11://12月
        {
            if( NewTime.tm_mday == 31 )
            {
                return true;
            }
        }
        break;	

    case 3://4月
    case 5://6月
    case 8://9月
    case 10://11月
        {
            if( NewTime.tm_mday == 30 )
            {
                return true;
            }
        }
        break;	
    }

    return false;
}

//取以nBegin开始、长度为nSize的一个随机不重复串
std::vector<int> ProduceRandSerial(int nBegin, int nSize)
{
    std::vector<int> vct(nSize);
    for (int i=0; i<nSize; ++i)
    {
        vct[i] = nBegin+i;
    }
    int nRand, nValue;
    for (int i=nSize; i>0; --i)
    {
        nRand = rand() % i;
        nValue = vct[nRand];
        vct.erase(vct.begin()+nRand);
        vct.push_back(nValue);
    }
    return vct;
}
#ifdef LIB_WINDOWS
BOOL WCharToMByte( LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, 0, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (dwSize < dwMinSize)
	{
		return FALSE;
	}
	WideCharToMultiByte(CP_OEMCP, 0, lpcwszStr, -1, lpszStr, dwSize, NULL, FALSE);
	return TRUE;
}

BOOL MByteToWChar(LPCSTR lpszStr, LPWSTR lpcwszStr, DWORD dwSize)
{
	DWORD dwMinSize;
	dwMinSize = MultiByteToWideChar(CP_OEMCP, 0, lpszStr, -1, NULL, 0);
	if (dwSize < dwMinSize)
	{
		return FALSE;
	}
	MultiByteToWideChar(CP_OEMCP, 0, lpszStr, -1, lpcwszStr, dwSize);
	return TRUE;
}
#endif // LIB_WINDOWS

#ifdef LIB_WINDOWS
BOOL WChar2Ansi( LPCWSTR lpcwszStr, LPSTR lpszStr, int nStrLenMax )
{
	// 	ASSERT(lpcwszStr != NULL);
	// 	ASSERT(lpszStr != NULL);
	int _convert = (int)wcslen(lpcwszStr)+1;
	if( _convert >= nStrLenMax )
		return FALSE;
	*lpszStr = '\0';
	int ret = WideCharToMultiByte( CP_ACP, 0, lpcwszStr, -1, lpszStr, nStrLenMax, NULL, NULL);
	if(ret == 0)
		return FALSE;
	return TRUE;
}
BOOL Ansi2WChar( LPCSTR lpszStr, LPWSTR lpwszStr, int nWStrLenMax )
{
	// 	ASSERT(lpwszStr != NULL);
	// 	ASSERT(lpszStr != NULL);
	int _convert = (int)strlen(lpszStr)+1;
	if( _convert >= nWStrLenMax )
		return FALSE;
	*lpwszStr = L'\0';
	int ret = MultiByteToWideChar( CP_ACP, 0, lpszStr, -1, lpwszStr, nWStrLenMax );
	if(ret == 0)
		return FALSE;
	return TRUE;
}
#else
BOOL WChar2Ansi(LPCWSTR lpcwszStr, LPSTR lpszStr, int nStrLenMax)
{
	setlocale(LC_ALL, "zh_CN.gbk");

	if (lpcwszStr == NULL)
	{
		return FALSE;
	}

	size_t size = wcslen(lpcwszStr)*sizeof(wchar_t);

	wcstombs(lpszStr, lpcwszStr, nStrLenMax);

	return TRUE;
}
BOOL Ansi2WChar( LPCSTR lpszStr, LPWSTR lpwszStr, int nWStrLenMax )
{
	setlocale(LC_ALL, "zh_CN.gbk");

	if (lpszStr == NULL)
	{
		return FALSE;
	}

	size_t size_of_ch = strlen(lpszStr)*sizeof(char);

	if (nWStrLenMax < size_of_ch)
	{
		return FALSE;
	}

	mbstowcs(lpwszStr, lpszStr, nWStrLenMax);

	return TRUE;
}
#endif // LIB_WINDOWS

#ifdef LIB_WINDOWS
//异常捕获回调函数
LONG __stdcall TheCrashHandlerFunction(EXCEPTION_POINTERS* pExPtrs)
{
	CExceptionLogger explogger;
	explogger.UnHandledExceptionFilter(pExPtrs);

	LOG("=============Exception================");
	return EXCEPTION_EXECUTE_HANDLER;
}

void LogRaiseException()
{
	//TRY_BEGIN_EXCEP
		RaiseException(0, 0, 0, 0);
	//TRY_END_EXCEP("RaiseException")
}
#endif // LIB_WINDOWS

void InitlogManager(LPCSTR pszFilePrefix)
{
	YTSvrLib::CProcessLogMgr::GetInstance()->Init(pszFilePrefix);
}
void DelLogManager()
{
	YTSvrLib::CProcessLogMgr::GetInstance()->ShutDown();
	YTSvrLib::CProcessLogMgr::DelInstance();
	CloseLog();
}

__time32_t SystemTimeToTime_t( LPSYSTEMTIME pst )
{
#ifdef LIB_WINDOWS
	FILETIME ft;
	if (FALSE == SystemTimeToFileTime(pst, &ft))
		return 0;
	FILETIME utcft;
	if (FALSE == LocalFileTimeToFileTime(&ft, &utcft))
		return 0;

	LONGLONG ll = ((LONGLONG) utcft.dwHighDateTime << 32) + utcft.dwLowDateTime;
	__time32_t tRet = (__time32_t) ((ll - 116444736000000000) / 10000000);
#else
	tm st;

	st.tm_year = pst->wYear - 1900;
	st.tm_mon = pst->wMonth - 1;
	st.tm_mday = pst->wDay;
	st.tm_hour = pst->wHour;
	st.tm_min = pst->wMinute;
	st.tm_sec = pst->wSecond;

	__time32_t tRet = mktime(&st);
#endif // LIB_WINDOWS
	return tRet;
}

int lwchartoutf8(LPCWSTR p, LPSTR pdst, int cbMultiByte)
{
	int count_bytes = 0;
	wchar_t byte_one = 0, byte_other = 0x3f; // 用于位与运算以提取位值 0x3f--->00111111
	unsigned char utf_one = 0, utf_other = 0x80; // 用于"位或"置标UTF-8编码 0x80--->1000000
	wchar_t tmp_wchar = 0; // 用于宽字符位置析取和位移(右移6位)
	unsigned char tmp_char = 0;

	int ret = 0;
	for (int i = 0; p[i] != 0 && ret < cbMultiByte; ++i)
	{
		if (p[i] <= 0x7f){ // <=01111111
			count_bytes = 1; // ASCII字符: 0xxxxxxx( ~ 01111111)
			byte_one = 0x7f; // 用于位与运算, 提取有效位值, 下同
			utf_one = 0x0;
		}
		else if ( (p[i] > 0x7f) && (p[i] <= 0x7ff) ){ // <=0111,11111111
			count_bytes = 2; // 110xxxxx 10xxxxxx[1](最多11个1位, 简写为11*1)
			byte_one = 0x1f; // 00011111, 下类推(1位的数量递减)
			utf_one = 0xc0; // 11000000
		}
		else if ( (p[i] > 0x7ff) && (p[i] <= 0xffff) ){ //0111,11111111<=11111111,11111111
			count_bytes = 3; // 1110xxxx 10xxxxxx[2](MaxBits: 16*1)
			byte_one = 0xf; // 00001111
			utf_one = 0xe0; // 11100000
		}
		else if ( (p[i] > 0xffff) && (p[i] <= 0x1fffff) ){ //对UCS-4的支持..
			count_bytes = 4; // 11110xxx 10xxxxxx[3](MaxBits: 21*1)
			byte_one = 0x7; // 00000111
			utf_one = 0xf0; // 11110000
		}
		else if ( (p[i] > 0x1fffff) && (p[i] <= 0x3ffffff) ){
			count_bytes = 5; // 111110xx 10xxxxxx[4](MaxBits: 26*1)
			byte_one = 0x3; // 00000011
			utf_one = 0xf8; // 11111000
		}
		else if ( (p[i] > 0x3ffffff) && (p[i] <= 0x7fffffff) ){
			count_bytes = 6; // 1111110x 10xxxxxx[5](MaxBits: 31*1)
			byte_one = 0x1; // 00000001
			utf_one = 0xfc; // 11111100
		}
		else{
			count_bytes = 1;
			byte_one = 0x7f; // 用于位与运算, 提取有效位值, 下同
			utf_one = 0x0;
		}


		// 以下几行析取宽字节中的相应位, 并分组为UTF-8编码的各个字节

		if (ret + count_bytes > cbMultiByte)
			return ret;

		tmp_wchar = p[i];
		for (int k = count_bytes; k > 1; k--)
		{ // 一个宽字符的多字节降序赋值
			tmp_char = (unsigned char)(tmp_wchar & byte_other);///后6位与byte_other 00111111
			pdst[ret + k - 1] = (tmp_char | utf_other);/// 在前面加10----跟10000000或
			tmp_wchar >>= 6;//右移6位
		}
		//这个时候i=1
		//对UTF-8第一个字节位处理，
		//第一个字节的开头"1"的数目就是整个串中字节的数目
		tmp_char = (unsigned char)(tmp_wchar & byte_one);//根据上面附值得来，有效位个数
		pdst[ret] = (tmp_char | utf_one);//根据上面附值得来 1的个数
		ret += count_bytes;
	}

	return ret;
}

int GetCPUCoreCount()
{
#ifdef LIB_WINDOWS
	SYSTEM_INFO si;

	GetSystemInfo(&si);

	return (int)si.dwNumberOfProcessors;
#else
	return get_nprocs();
#endif
}

DWORD WINAPI GetPrimeNumInRange(DWORD dwLow, DWORD dwHigh)
{
	for (DWORD i=dwLow; i<dwHigh; i++)
	{
		BOOL bCheck = TRUE;
		DWORD dwQqrt = (DWORD)sqrt((double)i);
		for (DWORD j=2; j<dwQqrt; j++)
		{
			if (i % j == 0)
			{
				bCheck = FALSE;
				break;
			}
		}
		if (bCheck)
			return i;
	}
	return 0;
}