AUTOMAKE_OPTIONS=foreign

ytsvrlibdir=$(libdir)

ytsvrlib_PROGRAMS=libytsvrlib.so

libytsvrlib_so_CPPFLAGS=-fpermissive -shared -fPIC -I$(BASEDIR)/mysql/include/ -std=c++11

libytsvrlib_so_LDFLAGS=-fpic -shared -levent -levent_pthreads -lmysqlclient -lcurl -lpthread -lmysqlpp -lhiredis -ltcmalloc_minimal

libytsvrlib_so_SOURCES = \
stdafx.cpp \
Base64/Base64.cpp \
Global/GlobalServer.cpp \
Global/Performance.cpp \
Global/GlobalURLRequest.cpp \
md5/md5.cpp \
mysql/MYSQLCommand.cpp \
mysql/MYSQLDBClient.cpp \
mysql/MYSQLDBSys.cpp \
mysql/MYSQLManagerBase.cpp \
Service/Application.cpp \
Service/AutoCloseFile.cpp \
Service/Log.cpp \
Service/PkgParserBase.cpp \
Service/YTThread.cpp \
Service/Utility.cpp \
Service/SyncObj.cpp \
Service/TimerThread.cpp \
Service/TimerHandler.cpp \
Socket/SendBuffer.cpp \
Socket/YTSocketBase.cpp \
Socket/YTSocketThread.cpp \
Socket/YTSocketConnector.cpp \
Socket/YTSocketClient.cpp \
Socket/YTSocketServer.cpp \
Socket/YTSocketMutiClient.cpp \
Socket/YTSocketMutiClientController.cpp \
tinyxml/tinyxml2.cpp \
tinyxml/XMLParser.cpp \
tinyxml/XMLDocument.cpp \
tinyxml/XMLNode.cpp \
urlwriter/urlwriter.cpp \
redis/RedisConnector.cpp \
redis/RedisConnectorPool.cpp \
redis/RedisConnectorCommand.cpp \
Filter/Filter.cpp

install-exec-hook:
	sudo ./install.sh
