#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([sample], [1.00])
AC_CONFIG_SRCDIR([stdafx.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
CXXFLAGS=${CXXFLAGS--g}
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(event,event_init,[],[
	echo "need libevent version >= 2.0.1"
	exit -1
])
AC_CHECK_LIB(pthread,pthread_create,[],[
	echo "need pthread install first"
	exit -1
])
AC_CHECK_LIB(mysqlclient,mysql_init,[],[
	echo "need libmysqlclient with mysql 5.6+"
	exit -1
])
AC_CHECK_LIB(curl,curl_global_init,[],[
	echo "need libcurl install first"
	exit -1
])

AC_CHECK_LIB(mysqlpp,_init,[],[
	echo "need mysqlpp install first"
	exit -1
])
AC_CHECK_LIB(ytsvrlib,get_version_code,[],[
	echo "cannot find ytsvrlib!!"
	exit -1
])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([floor gettimeofday memset pow sqrt strrchr])
AC_CHECK_FUNCS([localtime_r])
AC_CHECK_HEADERS([sys/time.h])
AC_FUNC_MKTIME
AC_FUNC_MALLOC
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T

AC_OUTPUT(Makefile)
