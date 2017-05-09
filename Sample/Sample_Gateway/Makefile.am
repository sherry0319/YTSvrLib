AUTOMAKE_OPTIONS=foreign

YTSVRLIB=/usr/include/ytsvrlib/

noinst_PROGRAMS=newromegateway

newromegateway_CPPFLAGS=-I$(YTSVRLIB)libevent/ -I$(YTSVRLIB) -I$(YTSVRLIB)mysql/include/ -std=c++11

newromegateway_LDFLAGS=-levent -lz -lpthread -lhiredis -lytsvrlib -ltcmalloc -rdynamic

newromegateway_SOURCES = stdafx.h \
stdafx.cpp \
Gateway.cpp \
ClientParser/GameSocket.cpp \
ClientParser/PkgParser.cpp \
Config/Config.cpp \
ServerParser/ServerParser.cpp \
ServerParser/ServerSocket.cpp \
timer/TimerMgr.cpp

install-exec-hook:
	cp -rfu newromegateway $(INSTALL_PATH)
