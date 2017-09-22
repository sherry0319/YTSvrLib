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
#ifndef _MYSQL_COMMAND_H_
#define _MYSQL_COMMAND_H_

#include <mysql.h>

#define MAX_QUERY_SIZE 1024 X64

namespace YTSvrLib
{
	class YTSVRLIB_EXPORT CMYSQLException
	{
	public:
		CMYSQLException(int nErrorCode, LPCSTR lpszErrorMessage)
		{
			m_nErrorCode = nErrorCode;
			strncpy(m_szErrorMessage, lpszErrorMessage, 127);
		}

		~CMYSQLException()
		{

		}

		int GetErrorCode() const
		{
			return m_nErrorCode;
		}
		LPCSTR GetError()
		{
			return m_szErrorMessage;
		}
	private:
		CHAR m_szErrorMessage[128];
		int m_nErrorCode;
	};


	class YTSVRLIB_EXPORT CMYSQLCommand :public CRecycle
	{
	public:
		CMYSQLCommand()
		{
			m_QueryInfo.clear();
			m_bExcute = FALSE;
		}

		~CMYSQLCommand()
		{

		}

		void AddQueryText(LPCWSTR lpwzFormat, ...);
		void AddQueryText(LPCSTR lpszFormat, ...);

		void AddQueryText_NoFormat(LPCSTR lpszSQL);
		void AddQueryText_NoFormat(LPCWSTR lpwzSQL);

		void AddQueryText_NoFormat(string& strSQL);
		void AddQueryText_NoFormat(wstring& wstrSQL);

		std::string GetSQLRequest()
		{
			return m_QueryInfo;
		}

		void SetExcute(BOOL bExcute = TRUE)
		{
			m_bExcute = bExcute;
		}
		BOOL IsExcute() const
		{
			return m_bExcute;
		}

		virtual void Init()
		{
			m_QueryInfo.clear();
			m_bExcute = FALSE;
		}
	private:
		std::string m_QueryInfo;
		BOOL m_bExcute;
	};


}

#endif