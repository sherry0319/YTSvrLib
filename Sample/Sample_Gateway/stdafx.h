// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "YTSvrLib.h"

#include <unordered_map>
#include <unordered_set>

#define USE_CHATSVR
#define USE_ADMINSVR

#include "../Common/GlobalDefine.h"
#include "../Common/MsgRetDef.h"
#include "../Common/ClientDef.h"
#include "Global.h"
#include "Config/Config.h"

#include "timer/TimerMgr.h"
#include "ServerParser/ServerParser.h"
#include "ClientParser/PkgParser.h"