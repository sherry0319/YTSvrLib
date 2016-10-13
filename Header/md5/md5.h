#ifndef _LGY_MD5_H
#define _LGY_MD5_H

void MD5( LPCWSTR pwzInfo , wchar_t wzOutBuf[34] );
void MD5( LPCSTR pszInfo,char szOutBuf[34] );

void MD5(LPCWSTR pwzInfo,size_t nSize, wchar_t wzOutBuf[34]);
void MD5(LPCSTR pszInfo, size_t nSize, char szOutBuf[34]);

#endif