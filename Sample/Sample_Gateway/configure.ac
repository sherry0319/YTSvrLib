#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([newromegateway], [1.70])
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

AC_CHECK_LIB(ytsvrlib,get_version_code,[],[
        echo "cannot find ytsvrlib!!"
        exit -1
])

AC_CHECK_LIB(z,compress,[],[
        echo "cannot find zlib!!"
        exit -1
])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memset strrchr])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([localtime_r])
AC_CHECK_HEADERS([sys/time.h])
AC_FUNC_MKTIME
AC_OUTPUT(Makefile)
