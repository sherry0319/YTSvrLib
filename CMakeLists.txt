CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

IF(WIN32)
	MESSAGE(FATAL_ERROR "YTSVRLIB CMAKE VERSION IS NOT SUPPORT WIN32&MSVC. PLEASE USE \"YTSvrLib.sln\" INSTEAD!!")
ENDIF()

PROJECT(ytsvrlib)

INCLUDE_DIRECTORIES(
src/
src/mysql/include/
)

OPTION(WITH_DEBUG "" OFF)

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
src/Service/PkgParserBase.cpp 
src/Service/YTThread.cpp 
src/Service/Utility.cpp 
src/Service/SyncObj.cpp 
src/Service/TimerThread.cpp 
src/Service/TimerHandler.cpp 
src/Socket/SendBuffer.cpp 
src/Socket/YTSocketBase.cpp 
src/Socket/YTSocketThread.cpp 
src/Socket/YTSocketConnector.cpp 
src/Socket/YTSocketClient.cpp 
src/Socket/YTSocketServer.cpp 
src/Socket/YTSocketMutiClient.cpp 
src/Socket/YTSocketMutiClientController.cpp
src/Socket/WebSocket/YTWSServer.cpp
src/Socket/WebSocket/YTWSConnector.cpp
src/sha/sha1.cpp
src/tinyxml/tinyxml2.cpp 
src/tinyxml/XMLParser.cpp 
src/tinyxml/XMLDocument.cpp 
src/tinyxml/XMLNode.cpp 
src/urlwriter/urlwriter.cpp 
src/redis/RedisConnector.cpp 
src/redis/RedisConnectorPool.cpp 
src/redis/RedisConnectorCommand.cpp 
src/Filter/Filter.cpp
)

SET(BUILD_SHARED_LIBS ON)

SET(CMAKE_CXX_FLAGS "-fstack-protector-all -fpermissive -std=c++11 -g")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

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
	event
	event_pthreads
	mysqlclient
	curl
	pthread
	mysqlpp
	hiredis
	websockets
	tcmalloc_minimal
	)
ENDIF()

INSTALL(CODE "execute_process\(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/install_cmake.sh\)")
