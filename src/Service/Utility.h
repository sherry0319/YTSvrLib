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
#ifndef __UTILITY_H_
#define __UTILITY_H_

#include <random>

//---------------------------------------
//
//	bool IsDirExist(char *lpszDir)
//
//	功能:	判断文件目录lpszDir是否存在
//
//	参数:	lpszDir	-- 目录字符串
//			
//
//	返回:	true -- 存在
//			false-- 不存在
//		
//---------------------------------------
YTSVRLIB_EXPORT bool IsDirExist(char *lpszDir);

//================================文件处理函数=======================================
#ifdef LIB_WINDOWS
YTSVRLIB_EXPORT inline void StringUpr(std::string& s)
{
	_strupr_s((char*)(s.c_str()), s.length());
}
YTSVRLIB_EXPORT inline void StringLwr(std::string& s)
{
	_strlwr_s((char*)(s.c_str()), s.length());
}
#endif // LIB_WINDOWS

//-------------------------------------------------
//
//	char* Trim(char *lpszStr)
//
//	功能:	截去字符串lpszStr头尾的空字符
//			空字符包括 ' ','\f','\r','\n','\t','\v'
//
//	参数:	lpszStr -- 要处理的字符串			
//
//	返回:	处理后的字串
//
//-------------------------------------------------
YTSVRLIB_EXPORT char* Trim(char* lpszStr);
YTSVRLIB_EXPORT wchar_t* Trim(wchar_t* lpswStr);
YTSVRLIB_EXPORT char* TrimR(char* lpszStr);
YTSVRLIB_EXPORT wchar_t* TrimR(wchar_t* lpswStr);
YTSVRLIB_EXPORT char* TrimL(char* lpszStr);
YTSVRLIB_EXPORT wchar_t* TrimL(wchar_t* lpswStr);

YTSVRLIB_EXPORT string& Trim(string& str);
YTSVRLIB_EXPORT wstring& Trim(wstring& str);
YTSVRLIB_EXPORT string& TrimR(string& str);
YTSVRLIB_EXPORT wstring& TrimR(wstring& str);
YTSVRLIB_EXPORT string& TrimL(string& str);
YTSVRLIB_EXPORT wstring& TrimL(wstring& str);

//--------------------------------------------------
//	功能:	以lpszDelimiter中包含的字符分割字串lpszStr,
//			得到的分割子串保存在字符串容器vecResult中
//
//	参数:	lpszStr			-- [in]要分割的字符串
//			lpszDelimiter	-- [in]分割字符串
//			vecResult		-- [out]返回的字符串容器
//
//	返回:	无
//		
//-----------------------------------------------
YTSVRLIB_EXPORT void StrDelimiter(const char *lpszSrc, const char *lpszDelimiter, std::vector<std::string> &vecResult);
YTSVRLIB_EXPORT void StrDelimiter(std::string& strSrc, const char* lpzDelimiter, std::vector<std::string> &vctResult);
YTSVRLIB_EXPORT void StrDelimiter(const char *lpszSrc, const char *lpszDelimiter, std::vector<int> &vctResult);
YTSVRLIB_EXPORT void StrDelimiter(std::string& strSrc, const char *lpszDelimiter, std::vector<int> &vctResult);

YTSVRLIB_EXPORT void StrDelimiter(const wchar_t* lpwzSrc, const wchar_t* lpwzDelimiter, std::vector<std::wstring> &vctResult);
YTSVRLIB_EXPORT void StrDelimiter(std::wstring& wstrSrc, const wchar_t* lpwzDelimiter, std::vector<std::wstring> &vctResult);
YTSVRLIB_EXPORT void StrDelimiter(const wchar_t* lpwzSrc, const wchar_t* lpwzDelimiter, std::vector<int> &vctResult);
YTSVRLIB_EXPORT void StrDelimiter(std::wstring& wstrSrc, const wchar_t* lpwzDelimiter, std::vector<int> &vctResult);
//拆分参数串


//	功能:	填充字符
//	参数:	pBuf			-- [in out]要填充的字符串,必须确保长度足够
//			cDelimiter		-- [in]分割标志字符
//			cFill			-- [int]填充的字符
YTSVRLIB_EXPORT void FillSpacing(char* pBuf, char cDelimiter, char cFill);

//-----------------------------------------------
//
//	char *DelSpace(char *s)
//
//	功能:
//		将字符串s中的前导空白符和尾随空白符删除，
//		并将字符串中间的连续多个空白符删减为一个空白符
//	参数:
//		s -- 要处理的字符串
//	返回:
//		处理后的字符串
//
//-----------------------------------------------
YTSVRLIB_EXPORT char *DelSpace(char *s);

//-----------------------------------------------
//
//	char *DelSpace(char *s)
//
//	功能:
//		将二进制数据转换为字串返回
//	参数:
//		pData -- 数据起始地址
//		len   -- 数据长度
//		delim -- 间隔字串
//	返回:
//		处理后的字符串
//
//-----------------------------------------------
YTSVRLIB_EXPORT std::string HexDump(const BYTE* pData, int len, const char *delim = " ");

//-----------------------------------------------
//	功能:
//		字串替换
//	参数:
//		strSource		-- 需要变换的原字串
//		strOldstring	-- 需要被替换的子字串
//		strNewstring	-- 替换的新字串
//	返回:
//		处理后的字符串
//
//-----------------------------------------------
YTSVRLIB_EXPORT void StrReplace(char *szSource, int nLen, const char *pszOldstring, const char *pszNewstring);
YTSVRLIB_EXPORT void StrReplace(wchar_t *wzSource, int nLen, const wchar_t *pwzOldstring, const wchar_t *pwzNewstring);

YTSVRLIB_EXPORT void StrReplace(std::string&strSource, const std::string&strOldstring, const std::string&strNewstring);
YTSVRLIB_EXPORT void StrReplace(std::wstring&strSource, const std::wstring&strOldstring, const std::wstring&strNewstring);

// 通配符字符串格式化.例如StrFormat("something {0} to {1} with {2}","123","kkk","???"); 将会得到something 123 to kkk with ???.注意通配符必须从{0}开始标.
YTSVRLIB_EXPORT std::string StrFormat(const char* format, ...);
YTSVRLIB_EXPORT std::string vStrFormat(const char* format, va_list va);

// 将任何类型转换为const char*类型
class YTSVRLIB_EXPORT string_coverter
{
public:
	string_coverter()
	{
		m_vctSave.clear();
	}

	virtual ~string_coverter()
	{
		for (size_t i = 0; i < m_vctSave.size();++i)
		{
			delete[] m_vctSave[i];
		}
		m_vctSave.clear();
	}

	template<typename T>
	const char* tostring(T value)
	{
		std::ostringstream s;
		s << value;
		char* buffer = new char[s.str().size() + 1];
		ZeroMemory(buffer, s.str().size() + 1);
#ifdef LIB_WINDOWS
		strncpy_s(buffer, (s.str().size()+1), s.str().c_str(), s.str().size());
#else
		strncpy_s(buffer, s.str().c_str(), s.str().size());
#endif
		m_vctSave.push_back(buffer);
		return buffer;
	}

private:
	std::vector<char*> m_vctSave;
};

#ifdef LIB_WINDOWS
YTSVRLIB_EXPORT BOOL WCharToMByte(LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);
YTSVRLIB_EXPORT BOOL MByteToWChar(LPCSTR lpszStr, LPWSTR lpcwszStr, DWORD dwSize);
#endif // LIB_WINDOWS

// 多字节强转ANSI格式字符串.注意.编码不换
YTSVRLIB_EXPORT BOOL WChar2Ansi(LPCWSTR lpcwszStr, LPSTR lpszStr, int nStrLenMax);
// ANSI字符串强转多字节字符串.注意.编码不换
YTSVRLIB_EXPORT BOOL Ansi2WChar(LPCSTR lpszStr, LPWSTR lpwszStr, int nWStrLenMax);

//================================数学计算函数================================
class RandomHelper
{
public:
	template<typename T>
	static T random_real(T min, T max)
	{
		std::uniform_real_distribution<T> dist(min, max);
		auto &mt = RandomHelper::getEngine();
		return dist(mt);
	}

	template<typename T>
	static T random_int(T min, T max)
	{
		std::uniform_int_distribution<T> dist(min, max);
		auto &mt = RandomHelper::getEngine();
		return dist(mt);
	}
private:
	static std::mt19937 &getEngine();
};

YTSVRLIB_EXPORT int Random2(int nMax, int nMin = 0); //介于 nMin~nMax-1的随机数
YTSVRLIB_EXPORT UINT Random2(UINT nMax, UINT nMin = 0); //介于 nMin~nMax-1的随机数
YTSVRLIB_EXPORT long Random2(long nMax, long nMin = 0); //介于 nMin~nMax-1的随机数
YTSVRLIB_EXPORT LONGLONG Random2(LONGLONG nMax, LONGLONG nMin = 0);//介于 nMin~nMax-1的随机数
YTSVRLIB_EXPORT DOUBLE Random2(DOUBLE dMax, DOUBLE dMin = 0.000000);//介于 dMin~dMax的随机数

//================================时间函数================================

//-----------------------------------------------------------
//
//	char* GetDateTime(char *lpszTimeBuf,char Flag='A')
//
//	功能:
//		得到当前日期时间保存到字符串lpszTimeBuf中
//	参数:
//		lpszTimeBuf	-- [out]要保存日期时间的字符串(长度要注意)	
//		Flag -- 'A' lpszTimeBuf = '2003-10-20 11:20:35_233212'
//				'S' lpszTimeBuf = '10-20 11:20:35_233212'
//				'D' lpszTimeBuf = '2003-10-20'
//				'T' lpszTimeBuf = '11:20:35_233212'
//				'W' lpszTimeBuf = '2003-10'
//				default lpszTimeBuf = '2003-10-20 11:20:35'
//	返回:
//		得到的时间日期字符串
//
//-----------------------------------------------------------
YTSVRLIB_EXPORT char* GetDateTime(char *lpszTimeBuf, int nLen, char Flag = 'A');
YTSVRLIB_EXPORT wchar_t* GetDateTime(wchar_t *lpwzTimeBuf, int nLen, char Flag = 'A');

//判断一天是不是当月的第一天
YTSVRLIB_EXPORT bool IsFirstDayOfMonth(const time_t& tTime);

//判断一天是不是当月的最后一天
YTSVRLIB_EXPORT bool IsLastDayOfMonth(const time_t& tTime);

//取以nBegin开始、长度为nSize的一个随机不重复串
YTSVRLIB_EXPORT std::vector<int> ProduceRandSerial(int nBegin, int nSize);

//捕获异常
#ifdef LIB_WINDOWS
class YTSVRLIB_EXPORT CWinsock
{
public:
	explicit CWinsock(BYTE mVers = '2', BYTE sVers = '2')
	{
		WSADATA      wsd;

		::WSAStartup(MAKEWORD(mVers, sVers), &wsd);
	}
	virtual ~CWinsock(void)
	{
		::WSACleanup();
	}
};

//异常捕获回调函数
YTSVRLIB_EXPORT LONG __stdcall TheCrashHandlerFunction(EXCEPTION_POINTERS * pExPtrs);

//================================捕获异常宏定义================================
//是否捕获异常
#define CATCH_EXCEPTION

#ifndef CATCH_EXCEPTION
#define TRY_BEGIN_EXCEP
#define TRY_END_EXCEP(symbol)
#else

#define TRY_BEGIN_EXCEP			__try {

#define TRY_END_EXCEP(symbol)		 }__except ( TheCrashHandlerFunction ( GetExceptionInformation ( ) ) )\
{\
    LOG( symbol);\
}
#endif

void LogRaiseException();

#define SELF_ASSERT(a, b)	{ if (!(a))	\
{	\
    LOG(__FILE__, __LINE__, "ASSERT报错");	\
    LogRaiseException();	\
    b;	\
}}
#else
#define TRY_BEGIN_EXCEP try{

#define TRY_END_EXCEP(symbol) }catch(...){LOG(symbol);}
#endif // LIB_WINDOWS

//延时日志
#define LOG_DECLARE_BEGIN	DWORD dwLogBeginTime = GetTickCount();
#define LOG_BEGIN			dwLogBeginTime = GetTickCount();
#define LOG_END(t, m)		if (GetTickCount()-dwLogBeginTime >= t)	\
{	\
    LOG(__FILE__, __LINE__, "%s : %d", m, GetTickCount()-dwLogBeginTime);	\
}




YTSVRLIB_EXPORT void InitlogManager(LPCSTR pszFilePrefix);
YTSVRLIB_EXPORT void DelLogManager();

YTSVRLIB_EXPORT __time32_t SystemTimeToTime_t(LPSYSTEMTIME pst);

YTSVRLIB_EXPORT int GetCPUCoreCount();// 获取cpu核心数

YTSVRLIB_EXPORT int lwchartoutf8(LPCWSTR p, LPSTR pdst, int cbMultiByte);
YTSVRLIB_EXPORT DWORD WINAPI GetPrimeNumInRange(DWORD dwLow, DWORD dwHigh);

#endif