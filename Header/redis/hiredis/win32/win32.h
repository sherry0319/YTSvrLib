#ifndef _WIN32_HELPER_INCLUDE
#define _WIN32_HELPER_INCLUDE
#ifdef _MSC_VER

#ifdef _LIB_SYS

#ifndef inline
#define inline __inline
#endif

#ifndef va_copy
#define va_copy(d,s) ((d) = (s))
#endif

#ifndef snprintf
#define snprintf c99_snprintf
#endif

#endif

int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap);

int c99_snprintf(char* str, size_t size, const char* format, ...);

#endif

#endif