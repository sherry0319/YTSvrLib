AUTOMAKE_OPTIONS=foreign

YTSVRLIB=/usr/include/ytsvrlib/

noinst_PROGRAMS=sample

sample_CPPFLAGS=-I$(YTSVRLIB)libevent/ -I$(YTSVRLIB) -I$(YTSVRLIB)mysql/include/ -std=c++11

sample_LDFLAGS=-lmysqlclient -levent -lcurl -lpthread -lmysqlpp -lhiredis -lytsvrlib -ltcmalloc

sample_SOURCES = \
stdafx.cpp \
Sample.cpp \
Config/Config.cpp \
db/DBLogMgr.cpp \
db/DBManager.cpp \
GWServerParser/GWServerParser.cpp \
GWServerParser/GWServerSocket.cpp \
timer/TimerMgr.cpp

install-exec-hook:
	cp -rfu sample $(INSTALL_PATH)
