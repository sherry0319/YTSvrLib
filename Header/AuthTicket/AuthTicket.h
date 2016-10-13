#pragma once

#define AUTH_TICKET_OK		0
#define AUTH_TICKET_EXPIRED	1
#define AUTH_TICKET_INVALID	2

/*
返回值 : 
0 = 成功
1 = 票据已过期
2 = 票据无效
*/
UINT CheckAuthTicket( __time32_t tNow,UINT nUserID,UINT nLastLogin, __time32_t tExpired, LPCWSTR pwzKey, LPCWSTR pwzTicket );
int	MakeAuthTicket( UINT nUserID, UINT nLastLogin, __time32_t tExpired, LPCWSTR pwzKey, wchar_t* pwzBuf, int nBufSize );

/*
返回值 :
0 = 成功
1 = 票据已过期
2 = 票据无效
*/
UINT CheckLoginTicket( UINT nUserID, UINT nGameID,  LPCWSTR pwzPartnerID, LPCWSTR pwzPassport, LPCWSTR pwzKey, LPCWSTR pwzTicket );
int	MakeLoginTicket( UINT nUserID, UINT nGameID,  LPCWSTR pwzPartnerID, LPCWSTR pwzPassport, LPCWSTR pwzKey, wchar_t* pwzBuf, int nBufSize );