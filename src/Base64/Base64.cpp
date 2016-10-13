/*MIT License

Copyright (c) 2016 Zhe Xu

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "stdafx.h"
#include "Base64.h"

/////////////////////////////////////////////////////////////////////////////
// Base64 encode/decode
//
static const char s_achBase64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

namespace YTSvrLib
{
	CBase64::CBase64()
	{
		Init();
	}

	void CBase64::Init()
	{
		int i = 0;
		for (i = 0; i < 256; i++)
			m_bBase64DecodeTable[i] = 0;

		BYTE bCode = 0;
		for (i = (int)'A'; i <= (int)'Z'; i++)
		{
			m_bBase64DecodeTable[i] = bCode;
			bCode++;
		}

		for (i = (int)'a'; i <= (int)'z'; i++)
		{
			m_bBase64DecodeTable[i] = bCode;
			bCode++;
		}

		for (i = '0'; i <= '9'; i++)
		{
			m_bBase64DecodeTable[i] = bCode;
			bCode++;
		}

		m_bBase64DecodeTable[(int)'+'] = bCode++;
		m_bBase64DecodeTable[(int)'/'] = bCode++;

		_ASSERTE(bCode == 64);
	}

	DWORD CBase64::CalcEncodeBufSize(DWORD dwDataLength)
	{
		return ((dwDataLength / 3) + 1) << 2;
	}

	size_t CBase64::Base64Encode(LPBYTE lpData, DWORD dwDataLength, LPBYTE lpBuf)
	{
		return GetInstance()->Encode(lpData, dwDataLength, lpBuf);
	}

	size_t CBase64::Encode(LPBYTE lpData, DWORD dwDataLength, LPBYTE lpBuf)
	{
		_ASSERTE(lpData);
		//_ASSERTE(dwBufLength >= XBase64CalcEncodeBufSize(lpData, dwDataLength));

		LPBYTE lpDataCur = lpData;
		LPBYTE lpBufCur = lpBuf;

		for (long i = 0; i < (long) dwDataLength / 3; ++i)
		{
			lpBufCur[0] = (BYTE) s_achBase64[(lpDataCur[0] & 0xFC) >> 2];
			lpBufCur[1] = (BYTE) s_achBase64[((lpDataCur[0] & 0x03) << 4) | ((lpDataCur[1] & 0xF0) >> 4)];
			lpBufCur[2] = (BYTE) s_achBase64[((lpDataCur[1] & 0x0F) << 2) | ((lpDataCur[2] & 0xC0) >> 6)];
			lpBufCur[3] = (BYTE) s_achBase64[lpDataCur[2] & 0x3F];

			lpBufCur += 4;
			lpDataCur += 3;
		}

		switch (dwDataLength % 3)
		{
			case 0:
				break;

			case 1:
				lpBufCur[0] = (BYTE) s_achBase64[(lpDataCur[0] & 0xFC) >> 2];
				lpBufCur[1] = (BYTE) s_achBase64[(lpDataCur[0] & 0x03) << 4];
				lpBufCur[2] = (BYTE)'=';
				lpBufCur[3] = (BYTE)'=';

				lpBufCur += 4;
				break;

			case 2:
				lpBufCur[0] = (BYTE) s_achBase64[(lpDataCur[0] & 0xFC) >> 2];
				lpBufCur[1] = (BYTE) s_achBase64[((lpDataCur[0] & 0x03) << 4) | ((lpDataCur[1] & 0xF0) >> 4)];
				lpBufCur[2] = (BYTE) s_achBase64[(lpDataCur[1] & 0x0F) << 2];
				lpBufCur[3] = (BYTE)'=';

				lpBufCur += 4;
				break;
		}

		return lpBufCur - lpBuf;
	}

	size_t CBase64::Base64Decode(LPBYTE lpBuf, DWORD dwBufLength, LPBYTE lpData)
	{
		return GetInstance()->Decode(lpBuf, dwBufLength, lpData);
	}

	size_t CBase64::Decode(LPBYTE lpBuf, DWORD dwBufLength, LPBYTE lpData)
	{
		_ASSERTE(dwBufLength % 4 == 0);

		LPBYTE lpBufCur = lpBuf;
		LPBYTE lpDataCur = lpData;

		for (DWORD i = 0; i < dwBufLength / 4; i++)
		{
			BYTE bA, bB, bC, bD;
			bA = m_bBase64DecodeTable[lpBufCur[0]];
			bB = m_bBase64DecodeTable[lpBufCur[1]];
			bC = m_bBase64DecodeTable[lpBufCur[2]];
			bD = m_bBase64DecodeTable[lpBufCur[3]];

			lpDataCur[0] = (bA << 2) | (bB >> 4);
			lpDataCur[1] = ((bB & 0x0F) << 4) | (bC >> 2);
			lpDataCur[2] = ((bC & 0x03) << 6) | bD;

			if (lpBufCur[2] != '=' && lpBufCur[3] != '=')
			{
				lpDataCur += 3;
			}
			else
			{
				if (lpBufCur[2] != '=')
				{
					lpDataCur[2] = 0;
					lpDataCur += 2;
				}
				else
				{
					lpDataCur[2] = 0;
					lpDataCur[1] = 0;
					lpDataCur += 1;
				}
			}

			lpBufCur += 4;
		}

		return lpDataCur - lpData;
	}
}