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
#include "MYSQLCommand.h"

namespace YTSvrLib
{

	void CMYSQLCommand::AddQueryText(LPCSTR lpszFormat, ...)
	{
		char szSQL[1024 * 16] = { 0 };
		va_list va;
		va_start(va, lpszFormat);
		_vsnprintf_s(szSQL, 1024 * 16, lpszFormat, va);
		va_end(va);

		m_QueryInfo = szSQL;
	}

	void CMYSQLCommand::AddQueryText(LPCWSTR lpwzFormat, ...)
	{
		WCHAR wzSQL[1024 * 8] = { 0 };
		va_list va;
		va_start(va, lpwzFormat);
		_vsnwprintf_s(wzSQL, 1024 * 8, lpwzFormat, va);
		va_end(va);

		char szSQL[1024 * 16] = { 0 };
		unicodetoutf8(wzSQL, szSQL, 1024 * 16);

		m_QueryInfo = szSQL;
	}

	void CMYSQLCommand::AddQueryText_NoFormat(LPCSTR lpszSQL)
	{
		m_QueryInfo = lpszSQL;
	}

	void CMYSQLCommand::AddQueryText_NoFormat(LPCWSTR lpwzSQL)
	{
		char szSQL[1024 * 16] = { 0 };
		unicodetoutf8(lpwzSQL, szSQL, 1024 * 16);

		m_QueryInfo = szSQL;
	}

	void MYSQLLIB::CMYSQLRecordSet::AddResult(const mysqlpp::StoreQueryResult& res)
	{
		res.enable_exceptions();

		m_CurQueryRes.push_back(res);
	}

	UINT MYSQLLIB::CMYSQLRecordSet::GetFieldValueBinary(const char* pszFieldName, char* pszOut, UINT nMaxLen)
	{
		if (pszFieldName == NULL)
		{
			throw CMYSQLException(-1, "Invalid pszFieldName NULL in GetFieldValue");
			return 0;
		}

		try
		{
			const mysqlpp::String& strField = m_CurQueryRes[m_nCurTable][m_nCurRow][pszFieldName];

			if (strField.is_null())
			{
				mysqlpp::mysql_type_info mysql_type = strField.type();
				const char* pszType = mysql_type.sql_name();

				if (strstr(pszType, "NULL"))
				{
					return 0;
				}
			}

			UINT nSize = (UINT) strField.size();
			if (nMaxLen < nSize)
			{
				return 0;
			}

			LPCSTR pszBuffer = strField.data();

			memcpy(pszOut, pszBuffer, nSize);

			return nSize;
		}
		catch (mysqlpp::BadFieldName& err)
		{
			char szError[1024] = { 0 };
			_snprintf_s(szError, 1023, "Invalid pszFieldName in GetFieldValue[%s]", err.what());
			throw CMYSQLException(-1, szError);
			return 0;
		}
	}
}