#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	int YTSVRLIB_EXPORT getopt_long_only(int nargc, char * const *nargv, const char *options, const struct option *long_options, int *idx);

	int YTSVRLIB_EXPORT getopt_long(int nargc, char * const *nargv, const char *options, const struct option *long_options, int *idx);

	int YTSVRLIB_EXPORT getopt(int nargc, char * const *nargv, const char *options);
#ifdef __cplusplus
}
#endif