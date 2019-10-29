AUTOMAKE_OPTIONS=foreign

ytsvrlibdir=$(libdir)

ytsvrlib_PROGRAMS=libytsvrlib.so

libytsvrlib_so_CPPFLAGS=-fpermissive -shared -fPIC -I$(BASEDIR)/mysql/include/ -std=c++11

libytsvrlib_so_LDFLAGS=-fpic -shared -liconv -lmysqlclient -lcurl -lpthread -lmysqlpp -lz -llz4 -lcpp_redis -ltacopie -ltcmalloc_minimal

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
mysql/MYSQLRecordSet.cpp \ 
Service/Application.cpp \ 
Service/AutoCloseFile.cpp \ 
Service/Log.cpp \ 
Service/YTThread.cpp \ 
Service/Utility.cpp \ 
Service/SyncObj.cpp \ 
Service/TimerThread.cpp \ 
Service/TimerHandler.cpp \ 
Socket/SendBuffer.cpp \ 
Socket/YTSocketThread.cpp \ 
Socket/TCPSocket/TCPSocket.cpp \ 
Socket/WebSocket/YTWSServer.cpp \ 
Socket/WebSocket/YTWSConnector.cpp \ 
Socket/WebSocket/WSSendBuffer.cpp \ 
sha/sha1.cpp \ 
tinyxml/tinyxml2.cpp \ 
tinyxml/XMLParser.cpp \ 
tinyxml/XMLDocument.cpp \ 
tinyxml/XMLNode.cpp \ 
urlwriter/urlwriter.cpp \ 
redis/RedisConnector.cpp \ 
redis/RedisConnectorPool.cpp\ 
Filter/Filter.cpp \ 
lz4/lz4feasy.cpp 

install-exec-hook:
	./install.sh
