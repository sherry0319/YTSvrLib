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

#include <mysql.h>

namespace YTSvrLib
{
	template<typename T>
	struct TypeAdapter
	{
		typedef T type;
	};

	namespace MYSQLLIB
	{
		class YTSVRLIB_EXPORT CMYSQLRecordSet :public CRecycle
		{
		public:
			CMYSQLRecordSet();

			virtual ~CMYSQLRecordSet();

			virtual void Init();

			void AddResult(const mysqlpp::StoreQueryResult& res);

			///移动到记录集首部
			void MoveFirst();

			///移动到记录集下一行
			void MoveNext();

			///移动到记录集上一行
			void MovePrevious();

			///移动到记录集尾部
			void MoveLast();

			///是否越过记录集末尾
			BOOL IsEof();

			void Close();

			///是否越过记录集首部
			BOOL IsBof();

			///得到记录条数
			int GetRecordCount();

			///得到记录集中字段的个数
			int GetFieldCount();

			std::string GetFieldName(int nIndex);

			LPCSTR GetFieldType(int nIndex);

			LPCSTR GetFieldValue(int nIndex, TypeAdapter<LPCSTR>);

			mysqlpp::DateTime GetFieldValue(int nIndex, TypeAdapter<mysqlpp::DateTime>);

			LPCSTR GetFieldValue(const char* pszFieldName, TypeAdapter<LPCSTR>);

			mysqlpp::DateTime GetFieldValue(const char* pszFieldName, TypeAdapter<mysqlpp::DateTime>);

			UINT GetFieldValueBinary(const char* pszFieldName, char* pszOut, UINT nMaxLen);

			BOOL NextRecordset();

			const mysqlpp::String& GetFieldString(int nIndex) const;

			const mysqlpp::String& GetFieldString(const char* pszFieldName) const;

			bool IsNull(const mysqlpp::String& strField) const;

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
					const mysqlpp::String& strField = GetFieldString(nIndex);

					if (IsNull(strField))
					{
						return _Ty(0);
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
					const mysqlpp::String& strField = GetFieldString(pszFieldName);

					if (IsNull(strField))
					{
						return _Ty(0);
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
		private:
			std::vector<mysqlpp::StoreQueryResult> m_CurQueryRes;
			int m_nCurTable;
			int m_nCurRow;
		};
	}
}