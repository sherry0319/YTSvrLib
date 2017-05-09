#pragma once

#ifdef LIB_WINDOWS
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif // LIB_WINDOWS

#define DEFAULT_KEEP_ALIVE_EXPIRED	90
#define HANGON_KEEP_ALIVE_EXPIRED	600

// #define USE_ADMINSVR
// 
// #define USE_CHATSVR

namespace EAppEvent
{
    enum 
    {
		eAppServerSocketEvent,
		eAppServerSocketDisconnectEvent,
// 		eAppAdminSocketEvent,
//         eAppAdminSocketDisconnectEvent,
        eAppClientSocketEvent,
        eAppClientSocketDisconnectEvent,
        eAppTimerMgrOnTimer,     
        eAppEventCount,
    };
};