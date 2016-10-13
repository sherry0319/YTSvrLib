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
#pragma once
#ifndef _MYSQL_COMMAND_H_
#define _MYSQL_COMMAND_H_

#include <mysql.h>

#define MAX_QUERY_SIZE 1024*64

namespace YTSvrLib
{
	class CMYSQLException
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


	class CMYSQLCommand :public CRecycle
	{
	public:
		CMYSQLCommand()
		{

		}

		~CMYSQLCommand()
		{

		}

		void AddQueryText(LPCWSTR lpwzFormat, ...);
		void AddQueryText(LPCSTR lpszFormat, ...);

		void AddQueryText_NoFormat(LPCSTR lpszSQL);
		void AddQueryText_NoFormat(LPCWSTR lpwzSQL);

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

	template<typename T>
	struct TypeAdapter
	{
		typedef T type;
	};

	namespace MYSQLLIB
	{
		class CMYSQLRecordSet :public CRecycle
		{
		public:
			CMYSQLRecordSet()
			{
				m_CurQueryRes.clear();
				m_nCurRow = 0;
				m_nCurTable = 0;
				ZeroMemory(wzPreData, sizeof(wzPreData));
			}

			~CMYSQLRecordSet()
			{
				m_CurQueryRes.clear();
			};

			virtual void Init()
			{
				Close();
				ZeroMemory(wzPreData, sizeof(wzPreData));
			}


			void AddResult(const mysqlpp::StoreQueryResult& res);

			//void SetAffectRows(ULONG nAffectRows){m_nAffectRows = nAffectRows;}

			///移动到记录集首部
			void MoveFirst()
			{
				m_nCurRow = 0;
			}

			///移动到记录集下一行
			void MoveNext()
			{
				m_nCurRow++;
				// 		if (m_nCurRow >= m_CurQueryRes[m_nCurTable].size())
				// 		{
				// 			m_nCurRow = m_CurQueryRes[m_nCurTable].size()-1;
				// 		}
			}

			///移动到记录集上一行
			void MovePrevious()
			{
				m_nCurRow--;
				// 		if (m_nCurRow < 0)
				// 		{
				// 			m_nCurRow = 0;
				// 		}
			}

			///移动到记录集尾部
			void MoveLast()
			{
				m_nCurRow = (int) (m_CurQueryRes[m_nCurTable].size() - 1);
			}

			///是否越过记录集末尾
			BOOL IsEof()
			{
				if (m_nCurRow >= (int)m_CurQueryRes[m_nCurTable].size())
				{
					return TRUE;
				}

				return FALSE;
			}

			void Close()
			{
				m_nCurRow = 0;
				m_nCurTable = 0;
				for (size_t i = 0; i < m_CurQueryRes.size(); ++i)
				{
					m_CurQueryRes[i].clear();
				}
				m_CurQueryRes.clear();
			}
			// 
			// 	void DeleteAll()
			// 	{
			// 		m_nCurTable = 0;
			// 		m_nCurRow = 0;
			// 		m_vctResult.clear();
			// 		m_vctFieldInfo.clear();
			// 		m_vceFieldName.clear();
			// 	}
			// 
			///是否越过记录集首部
			BOOL IsBof()
			{
				if (m_nCurRow < 0)
				{
					return TRUE;
				}

				return FALSE;
			}

			///得到记录条数
			int GetRecordCount()
			{
				return (int) m_CurQueryRes[m_nCurTable].size();
			}

			///得到记录集中字段的个数
			int GetFieldCount()
			{
				return (int) (m_CurQueryRes[m_nCurTable].num_fields());
			}

			std::string GetFieldName(int nIndex)
			{
				if (nIndex >= (int) m_CurQueryRes[m_nCurTable].num_fields())
				{
					return std::string("");
				}

				return m_CurQueryRes[m_nCurTable].field_name(nIndex);
			}

			LPCSTR GetFieldType(int nIndex)
			{
				if (nIndex >= (int) m_CurQueryRes[m_nCurTable].num_fields())
				{
					return "";
				}

				return m_CurQueryRes[m_nCurTable].field_type(nIndex).base_type().name();
			}

			template <typename _Ty>
			_Ty GetFieldValue(int nIndex)
			{
				return GetFieldValue(nIndex, TypeAdapter<_Ty>());
			}

			template<typename _Ty>
			_Ty GetFieldValue(int nIndex, TypeAdapter<_Ty>)
			{
				if (nIndex < 0 || nIndex >= m_CurQueryRes[m_nCurTable].num_fields())
				{
					throw CMYSQLException(-1, "Invalid nIndex in GetFieldValue");
					return _Ty(0);
				}

				try
				{
					const mysqlpp::String& strField = m_CurQueryRes[m_nCurTable][m_nCurRow][nIndex];

					if (strField.is_null())
					{
						mysqlpp::mysql_type_info mysql_type = strField.type();
						const char* pszType = mysql_type.sql_name();
						if (strstr(pszType, "NULL"))
						{
							return _Ty(0);
						}
					}

					return (_Ty) strField;
				}
				catch (mysqlpp::BadIndex& err)
				{
					char szError[1024] = { 0 };
					_snprintf_s(szError, 1023, "Invalid nIndex in GetFieldValue[%s]", err.what());
					throw CMYSQLException(-1, szError);
					return _Ty(0);
				}
			}

			LPCSTR GetFieldValue(int nIndex, TypeAdapter<LPCSTR>)
			{
				if (nIndex < 0 || nIndex >= (int) m_CurQueryRes[m_nCurTable].num_fields())
				{
					throw CMYSQLException(-1, "Invalid nIndex in GetFieldValue");
					return "";
				}

				try
				{
					const mysqlpp::String& strField = m_CurQueryRes[m_nCurTable][m_nCurRow][nIndex];

					if (strField.is_null())
					{
						mysqlpp::mysql_type_info mysql_type = strField.type();
						const char* pszType = mysql_type.sql_name();
						if (strstr(pszType, "NULL"))
						{
							return "";
						}
					}

					return (LPCSTR) strField;
				}
				catch (mysqlpp::BadIndex& err)
				{
					char szError[1024] = { 0 };
					_snprintf_s(szError, 1023, "Invalid nIndex in GetFieldValue[%s]", err.what());
					throw CMYSQLException(-1, szError);
					return "";
				}
			}

			LPCWSTR GetFieldValue(int nIndex, TypeAdapter<LPCWSTR>)
			{
				if (nIndex < 0 || nIndex >= (int) m_CurQueryRes[m_nCurTable].num_fields())
				{
					throw CMYSQLException(-1, "Invalid nIndex in GetFieldValue");
					return L"";
				}

				try
				{
					const mysqlpp::String& strField = m_CurQueryRes[m_nCurTable][m_nCurRow][nIndex];

					if (strField.is_null())
					{
						mysqlpp::mysql_type_info mysql_type = strField.type();
						const char* pszType = mysql_type.sql_name();
						if (strstr(pszType, "NULL"))
						{
							return L"";
						}
					}

					utf8tounicode((LPCSTR) strField, wzPreData, 1024 * 16);

					return wzPreData;
				}
				catch (mysqlpp::BadIndex& err)
				{
					char szError[1024] = { 0 };
					_snprintf_s(szError, 1023, "Invalid nIndex in GetFieldValue[%s]", err.what());
					throw CMYSQLException(-1, szError);
					return L"";
				}
			}

			mysqlpp::DateTime GetFieldValue(int nIndex, TypeAdapter<mysqlpp::DateTime>)
			{
				if (nIndex < 0 || nIndex >= (int) m_CurQueryRes[m_nCurTable].num_fields())
				{
					throw CMYSQLException(-1, "Invalid nIndex in GetFieldValue");
					return mysqlpp::DateTime((time_t) 0);
				}

				try
				{
					const mysqlpp::String& strField = m_CurQueryRes[m_nCurTable][m_nCurRow][nIndex];

					if (strField.is_null())
					{
						mysqlpp::mysql_type_info mysql_type = strField.type();
						const char* pszType = mysql_type.sql_name();
						if (strstr(pszType, "NULL"))
						{
							return mysqlpp::DateTime((time_t) 0);
						}
					}

					return (mysqlpp::DateTime)strField;
				}
				catch (mysqlpp::BadIndex& err)
				{
					char szError[1024] = { 0 };
					_snprintf_s(szError, 1023, "Invalid nIndex in GetFieldValue[%s]", err.what());
					throw CMYSQLException(-1, szError);
					return mysqlpp::DateTime((time_t) 0);
				}
			}

			template <typename _Ty>
			_Ty GetFieldValue(const char* pszFieldName)
			{
				return GetFieldValue(pszFieldName, TypeAdapter<_Ty>());
			}

			template<typename _Ty>
			_Ty GetFieldValue(const char* pszFieldName, TypeAdapter<_Ty>)
			{
				if (pszFieldName == NULL)
				{
					throw CMYSQLException(-1, "Invalid pszFieldName in GetFieldValue");
					return _Ty(0);
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
							return _Ty(0);
						}
					}

					return (_Ty) strField;
				}
				catch (mysqlpp::BadFieldName& err)
				{
					char szError[1024] = { 0 };
					_snprintf_s(szError, 1023, "Invalid pszFieldName in GetFieldValue[%s]", err.what());
					throw CMYSQLException(-1, szError);
					return _Ty(0);
				}
			}

			LPCSTR GetFieldValue(const char* pszFieldName, TypeAdapter<LPCSTR>)
			{
				if (pszFieldName == NULL)
				{
					throw CMYSQLException(-1, "Invalid pszFieldName in GetFieldValue");
					return "";
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
							return "";
						}
					}

					return (LPCSTR) strField;
				}
				catch (mysqlpp::BadFieldName& err)
				{
					char szError[1024] = { 0 };
					_snprintf_s(szError, 1023, "Invalid pszFieldName in GetFieldValue[%s]", err.what());
					throw CMYSQLException(-1, szError);
					return "";
				}
			}

			LPCWSTR GetFieldValue(const char* pszFieldName, TypeAdapter<LPCWSTR>)
			{
				if (pszFieldName == NULL)
				{
					throw CMYSQLException(-1, "Invalid pszFieldName in GetFieldValue");
					return L"";
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
							return L"";
						}
					}

					utf8tounicode((LPCSTR) strField, wzPreData, 1024 * 16);

					return wzPreData;

					return wzPreData;
				}
				catch (mysqlpp::BadFieldName& err)
				{
					char szError[1024] = { 0 };
					_snprintf_s(szError, 1023, "Invalid pszFieldName in GetFieldValue[%s]", err.what());
					throw CMYSQLException(-1, szError);
					return L"";
				}
			}

			mysqlpp::DateTime GetFieldValue(const char* pszFieldName, TypeAdapter<mysqlpp::DateTime>)
			{
				if (pszFieldName == NULL)
				{
					throw CMYSQLException(-1, "Invalid pszFieldName in GetFieldValue");
					return mysqlpp::DateTime((time_t) 0);
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
							return mysqlpp::DateTime((time_t) 0);
						}
					}

					return (mysqlpp::DateTime)strField;
				}
				catch (mysqlpp::BadFieldName& err)
				{
					char szError[1024] = { 0 };
					_snprintf_s(szError, 1023, "Invalid pszFieldName in GetFieldValue[%s]", err.what());
					throw CMYSQLException(-1, szError);
					return mysqlpp::DateTime((time_t) 0);
				}
			}

			UINT GetFieldValueBinary(const char* pszFieldName, char* pszOut, UINT nMaxLen);

			BOOL NextRecordset()
			{
				if (m_nCurTable >= (int) m_CurQueryRes.size())
				{
					return FALSE;
				}

				m_nCurTable++;
				m_nCurRow = 0;

				return TRUE;
			}


		private:
			std::vector<mysqlpp::StoreQueryResult> m_CurQueryRes;
			int m_nCurTable;
			int m_nCurRow;

			WCHAR wzPreData[1024 * 16];
		};
	};
}

#endif