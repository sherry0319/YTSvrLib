CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

IF(WIN32)
	MESSAGE(FATAL_ERROR "YTSVRLIB CMAKE VERSION IS NOT SUPPORT WIN32&MSVC. PLEASE USE \"YTSvrLib.sln\" INSTEAD!!")
ENDIF()

PROJECT(ytsvrlib)

INCLUDE_DIRECTORIES(
src/
src/mysql/include/
src/asio/
)

OPTION(WITH_DEBUG "Turn on debug for output symbol and more detailed log" OFF)

OPTION(WITH_WEBSOCKET "Turn on to enable websocket server" ON)

OPTION(WITH_REDIS "Turn on to enable redis connection and command" ON)

OPTION(WITH_HTTP "Turn on to enable http/https server" ON)

SET(
YTSVRLIB_SRC 
src/stdafx.cpp 
src/Base64/Base64.cpp 
src/Global/GlobalServer.cpp 
src/Global/Performance.cpp 
src/Global/GlobalURLRequest.cpp 
src/md5/md5.cpp 
src/mysql/MYSQLCommand.cpp 
src/mysql/MYSQLDBClient.cpp 
src/mysql/MYSQLDBSys.cpp 
src/mysql/MYSQLManagerBase.cpp 
src/mysql/MYSQLRecordSet.cpp 
src/Service/Application.cpp 
src/Service/AutoCloseFile.cpp 
src/Service/Log.cpp
src/Service/YTThread.cpp 
src/Service/Utility.cpp 
src/Service/SyncObj.cpp 
src/Service/TimerThread.cpp 
src/Service/TimerHandler.cpp 
src/Socket/SendBuffer.cpp 
src/Socket/YTSocketThread.cpp 
src/Socket/TCPSocket/TCPSocket.cpp
src/Socket/WebSocket/YTWSServer.cpp
src/Socket/WebSocket/YTWSConnector.cpp
src/Socket/WebSocket/WSSendBuffer.cpp
src/sha/sha1.cpp
src/tinyxml/tinyxml2.cpp 
src/tinyxml/XMLParser.cpp 
src/tinyxml/XMLDocument.cpp 
src/tinyxml/XMLNode.cpp 
src/urlwriter/urlwriter.cpp 
src/redis/RedisConnector.cpp 
src/redis/RedisConnectorPool.cpp
src/Filter/Filter.cpp
src/lz4/lz4feasy.cpp 
)

SET(BUILD_SHARED_LIBS ON)

SET(CMAKE_CXX_FLAGS "-fstack-protector-all -fpermissive -DASIO_STANDALONE -DUSE_STANDALONE_ASIO -D_WEBSOCKETPP_CPP11_TYPE_TRAITS_ -std=c++11 -g")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

IF(WITH_WEBSOCKET)
  ADD_DEFINITIONS(-DYTLIB_WITH_WEBSOCKET)
ENDIF()

IF(WITH_REDIS)
  ADD_DEFINITIONS(-DYTSVRLIB_WITH_REDIS)
  INCLUDE_DIRECTORIES(src/redis/)
ENDIF()

IF(WITH_HTTP)
  ADD_DEFINITIONS(-DYTSVRLIB_WITH_HTTP)
ENDIF()

IF(WITH_DEBUG)
  MESSAGE(STATUS "DEBUG MODE ON")
  FOREACH(LANG C CXX)
      SET(CMAKE_${LANG}_FLAGS
          "${CMAKE_${LANG}_FLAGS} -DDEBUG64 -O0")
  ENDFOREACH()
ELSE()
  MESSAGE(STATUS "DEBUG MODE OFF")
  FOREACH(LANG C CXX)
      SET(CMAKE_${LANG}_FLAGS
          "${CMAKE_${LANG}_FLAGS} -O2")
  ENDFOREACH()
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} ${YTSVRLIB_SRC})

IF(UNIX)
	TARGET_LINK_LIBRARIES(
	ytsvrlib
	z
	lz4
	mysqlclient
	curl
	pthread
	mysqlpp
	iconv
	)
	
	IF(WITH_REDIS)
		TARGET_LINK_LIBRARIES(
		ytsvrlib
		cpp_redis
		tacopie
		)
	ENDIF()
ENDIF()

INSTALL(CODE "execute_process\(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/install_cmake.sh\)")
