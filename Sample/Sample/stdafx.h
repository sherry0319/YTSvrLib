// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once



#if (defined(_WIN32) || defined(__WIN32__) || defined(_WIN64))
#include "targetver.h"
#pragma execution_character_set("utf-8")
#endif

//基础库的定义
#include "YTSvrLib.h"

#include "../Common/GlobalDefine.h"

#include "global/ServerDefine.h"
#include "global/MessageDefine.h"
#include "timer/TimerMgr.h"
#include "Config/Config.h"
#include "db/DBManager.h"
#include "GWServerParser/GWServerParser.h"
#include "timer/TimerMgr.h"
#include "db/DBLogMgr.h"