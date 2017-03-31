// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <stdlib.h>
#if defined(_WIN32) || defined(_WIN64)
#include <crtdbg.h>
#include <cstdarg>
#include "targetver.h"
// #include <afxwin.h>
// #include <afxtempl.h>
#endif // LIB_WINDOWS

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

// TODO: 在此处引用程序需要的其他头文件
//#include <afxwin.h>
using namespace std;
#include "YTSvrLib.h"

// #ifdef _WIN64
// #pragma comment(lib, "WS2_x64.Lib")
// #else
#ifdef LIB_WINDOWS
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Wldap32.lib")
#endif // LIB_WINDOWS

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




//#endif


