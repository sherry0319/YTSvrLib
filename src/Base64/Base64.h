/*MIT License

Copyright (c) 2016 Archer Xu

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

#pragma once

/////////////////////////////////////////////////////////////////////////////
// Base64 encode/decode
//

namespace YTSvrLib
{

	class CBase64 : public CSingle<CBase64>
	{
	public:
		CBase64();

		virtual ~CBase64(){};
	public:
		// 预估Base64后所需buffer长度
		static DWORD CalcEncodeBufSize(DWORD dwDataLength);

		// 加密
		static size_t Base64Encode(LPBYTE lpData, DWORD dwDataLength, LPBYTE lpBuf);
		size_t Encode(LPBYTE lpData, DWORD dwDataLength, LPBYTE lpBuf);

		// 解密
		static size_t Base64Decode(LPBYTE lpBuf, DWORD dwBufLength, LPBYTE lpData);
		size_t Decode(LPBYTE lpBuf, DWORD dwBufLength, LPBYTE lpData);
	private:
		// 初始化Base64
		void Init();

		BYTE m_bBase64DecodeTable[256];
	};
}