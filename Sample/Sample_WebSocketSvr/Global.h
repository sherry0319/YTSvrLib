#pragma once

#define DEFAULT_KEEP_ALIVE_EXPIRED	90
#define HANGON_KEEP_ALIVE_EXPIRED	600

namespace EAppEvent
{
    enum 
    {
		eAppServerSocketEvent,
        eAppClientSocketEvent,
        eAppTimerMgrOnTimer,     
        eAppEventCount,
    };
};