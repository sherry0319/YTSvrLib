// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#if (defined(_WIN32) || defined(__WIN32__) || defined(_WIN64))
#include "targetver.h"
#pragma execution_character_set("utf-8")
#endif

#include "YTSvrLib.h"

#define USE_CHATSVR
#define USE_ADMINSVR

#include "../Common/GlobalDefine.h"
#include "Global.h"
#include "Config/Config.h"

#include "timer/TimerMgr.h"
#include "ServerParser/ServerParser.h"
#include "ClientParser/PkgParser.h"
#include "ClientParser/MsgRetDef.h"