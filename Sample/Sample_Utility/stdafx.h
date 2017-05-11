// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include <iostream>


#include "YTSvrLib.h"

#ifdef _WIN64
#pragma comment(lib, "libcurl/libcurl64.lib")
#else
#pragma comment(lib, "libcurl/libcurl32.lib")
#endif

#pragma comment(lib, "iconv/lib/iconv.lib")

#ifdef _WIN64
#ifdef DEBUG64
#pragma comment(lib, "YTSvrLib_x64D.lib")
#else
#pragma comment(lib, "YTSvrLib_x64.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "YTSvrLib_x86D.lib")
#else
#pragma comment(lib, "YTSvrLib_x86.lib")
#endif
#endif
// TODO: 在此处引用程序需要的其他头文件
