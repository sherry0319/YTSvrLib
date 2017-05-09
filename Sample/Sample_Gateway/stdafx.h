// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include <stdio.h>

#if (defined(_WIN32) || defined(__WIN32__) || defined(_WIN64))

#include "targetver.h"
#include <tchar.h>
//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

// #include <afx.h>
// #include <afxwin.h>         // MFC 核心组件和标准组件
// #include <afxext.h>         // MFC 扩展
// #ifndef _AFX_NO_OLE_SUPPORT
// #include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
// #endif
// #ifndef _AFX_NO_AFXCMN_SUPPORT
// #include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
// #endif // _AFX_NO_AFXCMN_SUPPORT

// TODO: 在此处引用程序需要的其他头文件
#include <winsock2.h>
#include <Mswsock.h>
//#include <Windows.h>
//#include "afxtempl.h"
#include <process.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libevent/libevent_core.lib")
#pragma comment(lib, "BugReport/BugslayerUtil.lib")
#pragma comment(lib, "iconv/lib/iconv.lib")

#ifdef _WIN64
#pragma  comment(lib, "zlib/zlibmt_x64.lib")
#else
#pragma  comment(lib, "zlib/zlibmt_x86.lib")
#endif


#ifdef _WIN64
#pragma comment(lib, "libcurl/libcurl64.lib")
#else
#pragma comment(lib, "libcurl/libcurl32.lib")
#endif

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

#pragma comment(lib, "iconv/lib/iconv.lib")
#pragma comment( lib, "Winmm.lib" )
#endif // LIB_WINDOWS

#include "YTSvrLib.h"

#include <unordered_map>
#include <unordered_set>

#define USE_CHATSVR
#define USE_ADMINSVR

#include "../Common/GlobalDefine.h"
#include "Global.h"
#include "Config/Config.h"

#include "timer/TimerMgr.h"
#include "ServerParser/ServerParser.h"
#include "ClientParser/PkgParser.h"
#include "ClientParser/MsgRetDef.h"