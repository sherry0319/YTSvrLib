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
bool IsDirExist(char *lpszDir);

//================================文件处理函数=======================================
#ifdef LIB_WINDOWS
inline void StringUpr(std::string& s)
{
	_strupr_s((char*)(s.c_str()), s.length());
}
inline void StringLwr(std::string& s)
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
char* Trim(char* lpszStr );
wchar_t* TrimW(wchar_t* lpswStr );

//--------------------------------------------------
//
//	char* TrimR(char *lpszStr)
//
//	功能:	截去字符串lpszStr尾部的空字符
//			空字符包括 ' ','\f','\r','\n','\t','\v'
//
//	参数:	lpszStr -- 要处理的字符串
//
//	返回:	处理后的字串
//
//--------------------------------------------------
char* TrimR(char* lpszStr);
wchar_t* TrimRW(wchar_t* lpswStr);


//--------------------------------------------------
//
//	char* TrimL(char *lpszStr)
//
//	功能:	截去字符串lpszStr头部的空字符
//			空字符包括 ' ','\f','\r','\n','\t','\v'	
//
//	参数:	lpszStr -- 要处理的字符串	
//
//	返回:	处理后的字串
//
//--------------------------------------------------
char* TrimL(char* lpszStr );
wchar_t* TrimLW(wchar_t* lpswStr );

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
void StrDelimiter(const char *lpszSrc, const char *lpszDelimiter, std::vector<std::string> &vecResult);
void StrDelimiter( std::string& strSrc, const char* lpzDelimiter, std::vector<std::string> &vctResult );
void StrDelimiter(const char *lpszSrc, const char *lpszDelimiter, std::vector<int> &vctResult);
void StrDelimiter(std::string& strSrc, const char *lpszDelimiter, std::vector<int> &vctResult);

void StrDelimiter( const wchar_t* lpwzSrc, const wchar_t* lpwzDelimiter, std::vector<std::wstring> &vctResult );
void StrDelimiter( std::wstring& wstrSrc, const wchar_t* lpwzDelimiter, std::vector<std::wstring> &vctResult );
void StrDelimiter(const wchar_t* lpwzSrc, const wchar_t* lpwzDelimiter, std::vector<int> &vctResult);
void StrDelimiter( std::wstring& wstrSrc, const wchar_t* lpwzDelimiter, std::vector<int> &vctResult);
//拆分参数串


//	功能:	填充字符
//	参数:	pBuf			-- [in out]要填充的字符串,必须确保长度足够
//			cDelimiter		-- [in]分割标志字符
//			cFill			-- [int]填充的字符
void FillSpacing(char* pBuf, char cDelimiter, char cFill);

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
char *DelSpace(char *s);

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
std::string HexDump(const BYTE* pData, int len, const char *delim=" ");

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
void StrReplace(char *szSource, int nLen,const char *pszOldstring,const char *pszNewstring);
void StrReplaceW(wchar_t *wzSource, int nLen,const wchar_t *pwzOldstring,const wchar_t *pwzNewstring);

void StrReplace(std::string&strSource,const std::string&strOldstring,const std::string&strNewstring);
void StrReplaceW(std::wstring&strSource,const std::wstring&strOldstring,const std::wstring&strNewstring);

#ifdef LIB_WINDOWS
BOOL WCharToMByte( LPCWSTR lpcwszStr, LPSTR lpszStr, DWORD dwSize);
BOOL MByteToWChar(LPCSTR lpszStr, LPWSTR lpcwszStr, DWORD dwSize);
#endif // LIB_WINDOWS

// 多字节强转ANSI格式字符串.注意.编码不换
BOOL WChar2Ansi( LPCWSTR lpcwszStr, LPSTR lpszStr, int nStrLenMax );
// ANSI字符串强转多字节字符串.注意.编码不换
BOOL Ansi2WChar( LPCSTR lpszStr, LPWSTR lpwszStr, int nWStrLenMax );

template<class T>
T S2Var(const std::string& strValue)
{
    T Result;
    std::stringstream s;
    s = strValue;
    s >> Result;
    return Result;
}

template<class T>
std::string Var2S(const T nValue)
{
    std::string Result;
    std::stringstream s;
    s << nValue;
	Result = s;
    return Result;
}

//================================数学计算函数================================

//------------------------------------------------------
//
//	int Random(int nFirst,int nEnd)
//	
//	功能:	取nFirst到nEnd之间的随机数(包括nFirst,nEnd)
//		
//	参数:	nFirst -- 起始值
//			nEnd   -- 结束值	
//
//	返回:	int	-- 得到的随机数
//
//------------------------------------------------------
int Random(int nFirst,int nEnd);

//------------------------------------------------------
//
//	unsigned int Random(unsigned int nEnd)
//	
//	功能:	取0到nEnd-1之间的随机数
//		
//	参数:	nEnd   -- 结束值
//
//	返回:	unsigned int	-- 得到的随机数，该值低于0x1000
//
//------------------------------------------------------
unsigned int Random(unsigned int nEnd);

#ifdef LIB_WINDOWS
bool InitGenRandomFunction();
bool ReleaseGenRandomFunction();
#endif

int Random2( int nMax, int nMin = 0 ); //介于 nMin~nMax-1的随机数
LONGLONG Random2(LONGLONG nMax, LONGLONG nMin = 0);//介于 nMin~nMax-1的随机数
DOUBLE Random2(DOUBLE dMax, DOUBLE dMin = 0.000000, int nPrecision = 0);//介于 dMin~dMax的随机数.精度取nPrecision

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
char* GetDateTime(char *lpszTimeBuf, int nLen, char Flag='A');
wchar_t* GetDateTimeW(wchar_t *lpwzTimeBuf, int nLen, char Flag='A');

//判断一天是不是当月的第一天
bool IsFirstDayOfMonth( const time_t& tTime );

//判断一天是不是当月的最后一天
bool IsLastDayOfMonth( const time_t& tTime );

//取以nBegin开始、长度为nSize的一个随机不重复串
std::vector<int> ProduceRandSerial(int nBegin, int nSize);

//捕获异常
#ifdef LIB_WINDOWS
//异常捕获回调函数
LONG __stdcall TheCrashHandlerFunction ( EXCEPTION_POINTERS * pExPtrs );

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




void InitlogManager( LPCSTR pszFilePrefix );
void DelLogManager();

#ifdef LIB_WINDOWS
__time32_t SystemTimeToTime_t( LPSYSTEMTIME pst );
#endif

int GetCPUCoreCount();// 获取cpu核心数

int lwchartoutf8(LPCWSTR p, LPSTR pdst, int cbMultiByte);
DWORD WINAPI    GetPrimeNumInRange(DWORD dwLow, DWORD dwHigh);


#endif