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
#include <math.h>

//时间相关定义
#define SEC_MINUTE	(60) // 一分钟
#define SEC_HOUR	(60*SEC_MINUTE) //一小时
#define SEC_DAY		(24*SEC_HOUR) //一天
#define SEC_MONTH	(31*SEC_DAY) // 一个月
#define SEC_YEAR	(12*SEC_MONTH) //一年

#define SYS_WEEK_SUNDAY		0 //周日
#define SYS_WEEK_MONDAY		1 //周一
#define SYS_WEEK_TUESDAY	2 //周二
#define SYS_WEEK_WEDNESDAY	3 //周三
#define SYS_WEEK_THURSDAY	4 //周四
#define SYS_WEEK_FRIDAY		5 //周五
#define SYS_WEEK_SATURDAY	6 //周六

// 将两个不大于4字节的数值合称为一个8字节的数值
#define MAKELONGLONG(a, b)  ((LONGLONG)(((UINT)(((LONGLONG)(a)) & 0xffffffff)) | ((LONGLONG)((UINT)(((LONGLONG)(b)) & 0xffffffff))) << 32))
// 取8字节数中的地位
#define LOUINT(l)           ((UINT)(((LONGLONG)(l)) & 0xffffffff))
// 取8字节数中的高位
#define HIUINT(l)           ((UINT)((((LONGLONG)(l)) >> 32) & 0xffffffff))

// 取最小值
template<typename _Ty>
inline _Ty _min(_Ty nLeft, _Ty nRight)
{
	return (nLeft < nRight) ? nLeft : nRight;
}

// 取最大值
template<typename _Ty>
inline _Ty _max(_Ty nLeft, _Ty nRight)
{
	return (nLeft > nRight) ? nLeft : nRight;
}

// 交换两值
template<typename _Ty>
inline void _swap(_Ty& nLeft, _Ty& nRight)
{
	_Ty tmp;
	tmp = nLeft;
	nLeft = nRight;
	nRight = tmp;
}

typedef enum
{
	eInvalid, 
	eDisconnect, 
	eConnect, 
	eConnectFail, 
	eAccepted, 
	eCount
} EType;

typedef	std::vector<std::string> CStringVct;

enum EM_LOG_LEVEL
{
	LOG_LEVEL_TRACE,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_WARN,
	LOG_LEVEL_ERROR,
};

void LogBin( const char* pszData, int nDataLen );
void LogErrorASync(const char* szFile, const int nLine, EM_LOG_LEVEL emLevel, const char *fmt, ...);
void  LogError( const char* szFile, const int nLine, const char *fmt, ...);
void  LogError( const char *fmt, ...);
void  LogCommon( const char *fmt, ... );


#ifdef LIB_WINDOWS
#define LOG(s, ...) LogErrorASync( __FILE__, __LINE__,LOG_LEVEL_DEBUG, s, __VA_ARGS__)
#define LOGWARN(s,...) LogErrorASync( __FILE__, __LINE__,LOG_LEVEL_WARN, s, __VA_ARGS__)
#define LOGERROR(s,...) LogErrorASync( __FILE__, __LINE__,LOG_LEVEL_ERROR, s, __VA_ARGS__)
//#define LOG(s, ...) LogError( __FILE__, __LINE__,s, __VA_ARGS__)
#ifdef _DEBUG
#define LOGTRACE(s, ...) LogErrorASync( __FILE__, __LINE__, LOG_LEVEL_DEBUG, s, __VA_ARGS__)
#else
#define LOGTRACE(s, ...) LogErrorASync( __FILE__, __LINE__, LOG_LEVEL_TRACE, s, __VA_ARGS__)
#endif

#define LOGCOMMON(s, ...) LogCommon( s, __VA_ARGS__)
#else
#define LOG(s, args...) LogErrorASync( __FILE__, __LINE__,LOG_LEVEL_DEBUG, s, ##args)
#define LOGWARN(s,args...) LogErrorASync( __FILE__, __LINE__,LOG_LEVEL_WARN, s, ##args)
#define LOGERROR(s,args...) LogErrorASync( __FILE__, __LINE__,LOG_LEVEL_ERROR, s, ##args)
//#define LOG(s, ...) LogError( __FILE__, __LINE__,s, __VA_ARGS__)
#ifdef _DEBUG
#define LOGTRACE(s, args...) LogErrorASync( __FILE__, __LINE__, LOG_LEVEL_DEBUG, s, ##args)
#else
#define LOGTRACE(s, args...) LogErrorASync( __FILE__, __LINE__, LOG_LEVEL_TRACE, s, ##args)
#endif

#define LOGCOMMON(s, args...) LogCommon( s, ##args)
#endif // LIB_WINDOWS

void ReOpenLogFile();
void OpenLog();
void CloseLog();
BOOL IsOpenLog();

//////////////////////////////////////////////////////////////////////////

static inline BYTE toHex(const BYTE &x)
{
	return x > 9 ? x -10 + 'A': x + '0';
}

static inline BYTE fromHex(const BYTE &x)
{
	return isdigit(x) ? x-'0' : x-'A'+10;
}

// URL加密
inline std::string URLEncode(const std::string &sIn)
{
	std::string sOut;
	for( size_t ix = 0; ix < sIn.size(); ix++ )
	{      
		BYTE buf[4];
		memset( buf, 0, 4 );
		if( isalnum( (BYTE)sIn[ix] ) )
		{      
			buf[0] = sIn[ix];
		}
		//else if ( isspace( (BYTE)sIn[ix] ) ) //把空格编码成%20或者+都可以
		//{
		//    buf[0] = '+';
		//}
		else
		{
			buf[0] = '%';
			buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
			buf[2] = toHex( (BYTE)sIn[ix] % 16);
		}
		sOut += (char *)buf;
	}
	return sOut;
};

// URL解密
inline std::string URLDecode(const std::string &szToDecode)
{
	std::string result;  
	int hex = 0;  
	for (size_t i = 0; i < szToDecode.length(); ++i)  
	{  
		switch (szToDecode[i])  
		{  
		case '+':  
			result += ' ';  
			break;  
		case '%':  
			if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))  
			{
				std::string hexStr = szToDecode.substr(i + 1, 2);  
				hex = strtol(hexStr.c_str(), 0, 16);
				//字母和数字[0-9a-zA-Z]、一些特殊符号[$-_.+!*'(),] 、以及某些保留字[$&+,/:;=?@]  
				//可以不经过编码直接用于URL  
				if (!((hex >= 48 && hex <= 57) || //0-9  
					(hex >=97 && hex <= 122) ||   //a-z  
					(hex >=65 && hex <= 90)     //A-Z  
					//一些特殊符号及保留字[$-_.+!*'(),]  [$&+,/:;=?@]  
					/*|| hex == 0x21 || hex == 0x27 || hex == 0x28 || hex == 0x29 
					|| hex == 0x2a || hex == 0x2d || hex == 0x2e
					|| hex == 0x5f */
					))  
				{  
					result += char(hex);
					i += 2;  
				}  
				else result += '%';  
			}else {  
				result += '%';  
			}  
			break;  
		default:
			result += szToDecode[i];  
			break;  
		}  
	}  
	return result;
}

#ifdef LIB_LINUX
// 确保路径存在.不存在则创建文件夹
BOOL MakeSureDirectoryPathExists( LPCSTR DirPath );
// 获取文件大小
size_t GetFileSize(FILE* pFile);
// 输出当前堆栈信息到日志
void PrintBackTrace();
// 信号屏蔽
bool BlockSignal();
// 设置信号处理函数
typedef void(*signal_handle)(int);
void SetConsoleCtrlHandler(signal_handle handle);
#endif // LIB_LINUX

#ifdef LIB_WINDOWS

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS 11644473600000000ULL
#endif

struct timezone 
{  
	int  tz_minuteswest; // minutes W of Greenwich  
	int  tz_dsttime;     // type of dst correction
};

// Windows版gettimeofday
int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif // LIB_WINDOWS

// 安全关闭当前进程[带垃圾清理]
void SafeTerminateProcess();

// UTF8字符串转换为UNICODE字符串
int utf8tounicode(const char *utf8_buf, wchar_t* unicode_buf, int max_size);
// UNICODE字符串转换为UTF8字符串
int unicodetoutf8(const wchar_t* unicode_buf, char* utf8_buf,int max_size);

// 获取tNow之后的下一个星期wWeekDay的wHour时的UTC时间.(24小时制)
// 例如GetNextWeekDayTime(tNow,SYS_WEEK_SUNDAY,2)表示获取下一个周日2点的时间
__time32_t GetNextWeekDayTime(__time32_t tNow, WORD wWeekDay, WORD wHour);

// 计算出明天的年月日所表达的日期.例如20110802
int CalcTomorrowYYYYMMDD();
// 获取当前进程所在路径
void GetModuleFilePath(char* pszOut, int nLen);
// 获取当前进程名称
void GetModuleFileName(char* pszOut,int nLen);

// 将一个UTC时间转换为字符串格式的时间(2011-09-02 22:11:02)
// 特殊 : 0 = (0000-00-00 00:00:00)
const wchar_t* CovertUTC2String(__time32_t tTime, wchar_t* pwzOut, int nOutMaxLen);
const char* CovertUTC2String(__time32_t tTime, char* pszOut, int nOutMaxLen);

//把 年|月|日|时|分|秒 的时间转换为时间
__time32_t MakeStrTimeToUTC(LPCWSTR lpwzTime);
__time32_t MakeStrTimeToUTC(LPCSTR lpwzTime);

//把 月|日|时|分|秒 的时间依据给定的年份转换为时间
__time32_t MakeStrTimeToUTC_NoYear(LPCWSTR lpwzTime, UINT nYear);
__time32_t MakeStrTimeToUTC_NoYear(LPCSTR lpwzTime, UINT nYear);

// 生成一串随机的字符串
LPCSTR MakeRandomKey(LPSTR pszOut, UINT nOutMaxLen,UINT nNeedLen);
LPCWSTR MakeRandomKey(LPWSTR pszOut, UINT nOutMaxLen,UINT nNeedLen);

// 给引号(')和双引号(")之前添加转义斜杠(\)
// 注意.不要多次调用.已经添加的还会再添加一次
void AddSlashes(const wchar_t* pwzIn, wchar_t* pwzOut, int nLen);
void AddSlashes(const char* pszIn, char* pszOut, int nLen);

// 将 xx|xx|xx 格式的礼包打散成vector
void WINAPI ParseListStr(LPCWSTR pwzList, std::vector< std::vector<int> >& vctOut);
void WINAPI ParseListStr(LPCSTR pszList, std::vector< std::vector<int> >& vctOut);

// 将字符串全部转换为小写
int StringToLowcase(LPCWSTR lpwzSrc, LPWSTR lpwzOut, int nLen);
int StringToLowcase(LPCSTR lpszSrc, LPSTR lpszOut, int nLen);

//移除字符串前后空格
void RemoveSpace(LPCWSTR pwzSrc, LPWSTR pwzDst, int nLen);
void RemoveSpace(LPCSTR pszSrc, LPSTR pszDst, int nLen);

// 获取本机IP
void GetLocalIP(vector<string>& vctIPList);

// 检查是否是debug版本
bool CheckDebugVersion();
// 检查是否是零时区
bool CheckTimezoneZero();
