CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

PROJECT(sample_websocketsvr)

SET(YTSVRLIB "/usr/include/ytsvrlib/")

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}
${YTSVRLIB}
${YTSVRLIB}libevent/
${YTSVRLIB}mysql/include/
)

OPTION(WITH_DEBUG "" OFF)

SET(
SVR_SRC 
stdafx.cpp
WSGateway.cpp
ClientParser/GameSocket.cpp
ClientParser/PkgParser.cpp
Config/Config.cpp
ServerParser/ServerParser.cpp
ServerParser/ServerSocket.cpp
timer/TimerMgr.cpp
)

SET(CMAKE_CXX_FLAGS "-fstack-protector-all -fpermissive -std=c++11 -g")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")

IF(WITH_DEBUG)
  MESSAGE(STATUS "DEBUG MODE ON")
  FOREACH(LANG C CXX)
      SET(CMAKE_${LANG}_FLAGS
          "${CMAKE_${LANG}_FLAGS} -DDEBUG64 -O0")
  ENDFOREACH()
  
  INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/${PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../linux_output/)
ELSE()
  MESSAGE(STATUS "DEBUG MODE OFF")
  FOREACH(LANG C CXX)
      SET(CMAKE_${LANG}_FLAGS
          "${CMAKE_${LANG}_FLAGS} -O2")
  ENDFOREACH()
  
  INSTALL(PROGRAMS ${CMAKE_BINARY_DIR}/${PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../linux_output_release/)
ENDIF()

ADD_EXECUTABLE(${PROJECT_NAME} ${SVR_SRC})

IF(UNIX)
	TARGET_LINK_LIBRARIES(
	${PROJECT_NAME}
	ytsvrlib
	z
	event
	event_pthreads
	mysqlclient
	curl
	pthread
	mysqlpp
	hiredis
	tcmalloc_minimal
	)
ENDIF()

