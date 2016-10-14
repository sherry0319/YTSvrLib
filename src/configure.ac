#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([ytsvrlib], [1.0])
AC_CONFIG_SRCDIR([YTSvrLib.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
CXXFLAGS=${CXXFLAGS--g}
AC_PROG_RANLIB
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h fenv.h float.h inttypes.h limits.h locale.h malloc.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/ioctl.h sys/socket.h sys/time.h unistd.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])
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

AC_CHECK_LIB(hiredis,redisConnect,[],[
        echo "need hiredis install first"
        exit -1
])

# Checks for library functions.
AC_CHECK_FUNCS([gethostbyname gethostname getpagesize gettimeofday inet_ntoa localtime_r memchr memmove memset mkdir modf rint select setlocale socket sqrt stpcpy strcasecmp strchr strerror strncasecmp strpbrk strrchr strspn strstr strtol strtoull])

AC_OUTPUT(Makefile)
