AUTOMAKE_OPTIONS=foreign

YTSVRLIB=/usr/include/ytsvrlib/

noinst_PROGRAMS=newromedbsaver

newromedbsaver_CPPFLAGS=-I$(YTSVRLIB) -I$(YTSVRLIB)libevent/ -I$(YTSVRLIB)mysql/include/ -std=c++11

newromedbsaver_LDFLAGS=-lmysqlclient -levent -lcurl -lpthread -lmysqlpp -lhiredis -lytsvrlib -ltcmalloc -rdynamic

newromedbsaver_SOURCES = stdafx.h \
stdafx.cpp \
DBSaver.cpp \
Config/Config.cpp \
db/DBLogMgr.cpp \
db/DBManager.cpp \
dbcache/DBCache.cpp \
GMSvrParser/GMSvrSocket.cpp \
GMSvrParser/GMSvrParser.cpp \
timer/TimerMgr.cpp

install-exec-hook:
	cp -rfu newromedbsaver $(INSTALL_PATH)
