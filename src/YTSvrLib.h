/*MIT License

Copyright (c) 2016 Zhe Xu

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

#ifndef _DIPAN_SERVER_H
#define _DIPAN_SERVER_H

//#define TIXML_USE_STL
#define CURL_STATICLIB // 使用静态编译curl
#define MYSQLPP_NO_DLL // 使用静态编译mysqlpp

//编译时显示代码中的TODO列表和ATTENTION列表
//#pragma INFO("your information")
//#pragma TODO("your todo sth.")
//#pragma ATTENTION("your attention sth.")
#define TOSTR(x)        #x
#define TOSTR2(x)       TOSTR(x)
#define INFO(x)         message(__FILE__ "(" TOSTR2(__LINE__) "): INFO " x)
#define TODO(x)         message(__FILE__ "(" TOSTR2(__LINE__) "): TODO " x)
#define ATTENTION(x)    message(__FILE__ "(" TOSTR2(__LINE__) "): ATTENTION " x)

#if defined(_WIN32) || defined(_WIN64)
#define LIB_WINDOWS
#else
#define LIB_LINUX
#endif
using namespace std;
#ifdef LIB_WINDOWS
#include <winsock2.h>
#include <Mswsock.h>
#include <process.h>
#include "iconv/iconv.h"
#include "event.h"
EXTERN_C
{
#include "redis/hiredis/win32/hiredis.h"
#include "redis/hiredis/win32/async.h"
#include "redis/hiredis/win32/adapters/libevent.h"
}
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <malloc.h>
#include <cassert>
#include <cstdio>
#include <cwchar>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <pthread.h>
#include <iconv.h>
#include <netdb.h>
#include "typedef.h"
#include "event.h"
extern "C"
{
#include "redis/hiredis/linux/hiredis.h"
#include "redis/hiredis/linux/async.h"
#include "redis/hiredis/linux/adapters/libevent.h"
}
#endif // LIB_WINDOWS

//STL
#include <map>
#include <set>
#include <string>
#include <vector>
#include <list>
#include <queue>

#ifdef LIB_WINDOWS
#include "BugReport/BugslayerUtil.h"
#endif // LIB_WINDOWS

#ifdef LIB_LINUX
#include <wchar.h>
#include <locale.h>
#endif // LIB_LINUX

#include "./Global/GlobalServer.h"
#include "./Global/Perfermance.h"
#include "./Single/Single.h"
#include "./Service/SyncObj.h"
#include "./Service/Pool.h"
#include "./Service/Utility.h"
#include "./Service/Log.h"
#include "./Service/Application.h"
#include "./Service/AutoCloseFile.h"
#include "./Service/PkgParserBase.h"
#include "./Service/YTThread.h"
#include "./urlwriter/urlwriter.h"
#include "./mysql/MYSQLManagerBase.h"
#include "./mysql/MYSQLDBSys.h"
#include "./Base64/Base64.h"
#include "./md5/md5.h"
#include "./Filter/Filter.h"
#include "./tinyxml/XMLParser.h"
#include "./libcurl/curl.h"
#include "./zlib/zlib.h"
#include "./jsoncpp/json/json.h"
#include "./redis/RedisConnector.h"
#include "./Socket/YTSocketBase.h"
#include "./Socket/YTSocketThread.h"
#include "./Service/TimerThread.h"
#include "./Socket/YTSocketConnector.h"
#include "./Socket/YTSocketClient.h"
#include "./Socket/YTSocketServer.h"
#include "./Socket/YTSocketMutiClient.h"
#include "./Socket/YTSocketMutiClientController.h"

#endif