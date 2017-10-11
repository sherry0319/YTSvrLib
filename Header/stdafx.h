// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <cstdlib>
#if defined(_WIN32) || defined(_WIN64)
#include "targetver.h"
#include <cstdarg>
#endif // LIB_WINDOWS

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

using namespace std;
#include "YTSvrLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




//#endif


