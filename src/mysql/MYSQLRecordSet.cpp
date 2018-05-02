#include "stdafx.h"
#include "MYSQLCommand.h"

namespace YTSvrLib
{
	namespace MYSQLLIB
	{
		CMYSQLRecordSet::CMYSQLRecordSet()
		{
			m_CurQueryRes.clear();
			m_CurQueryRes.shrink_to_fit();
			m_nCurRow = 0;
			m_nCurTable = 0;
		}

		CMYSQLRecordSet::~CMYSQLRecordSet()
		{
			m_CurQueryRes.clear();
			m_CurQueryRes.shrink_to_fit();
		};

		void CMYSQLRecordSet::Init()
		{
			Close();
		}

		void CMYSQLRecordSet::MoveFirst()
		{
			m_nCurRow = 0;
		}

		void CMYSQLRecordSet::MoveNext()
		{
			m_nCurRow++;
		}

		void CMYSQLRecordSet::MovePrevious()
		{
			m_nCurRow--;
		}

		void CMYSQLRecordSet::MoveLast()
		{
			m_nCurRow = (int) (m_CurQueryRes[m_nCurTable].size() - 1);
		}

		void CMYSQLRecordSet::AddResult(const mysqlpp::StoreQueryResult& res)
		{
			res.enable_exceptions();

			m_CurQueryRes.push_back(res);
		}

		BOOL CMYSQLRecordSet::IsEof()
		{
			if (m_nCurRow >= (int) m_CurQueryRes[m_nCurTable].size())
			{
				return TRUE;
			}

			return FALSE;
		}

		void CMYSQLRecordSet::Close()
		{
			m_nCurRow = 0;
			m_nCurTable = 0;
			for (size_t i = 0; i < m_CurQueryRes.size(); ++i)
			{
				m_CurQueryRes[i].clear();
			}
			m_CurQueryRes.clear();
		}

		///是否越过记录集首部
		BOOL CMYSQLRecordSet::IsBof()
		{
			if (m_nCurRow < 0)
			{
				return TRUE;
			}

			return FALSE;
		}

		///得到记录条数
		int CMYSQLRecordSet::GetRecordCount()
		{
			return (int) m_CurQueryRes[m_nCurTable].size();
		}

		///得到记录集中字段的个数
		int CMYSQLRecordSet::GetFieldCount()
		{
			return (int) (m_CurQueryRes[m_nCurTable].num_fields());
		}

		std::string CMYSQLRecordSet::GetFieldName(int nIndex)
		{
			if (nIndex >= (int) m_CurQueryRes[m_nCurTable].num_fields())
			{
				return std::string("");
			}

			return m_CurQueryRes[m_nCurTable].field_name(nIndex);
		}

		LPCSTR CMYSQLRecordSet::GetFieldType(int nIndex)
		{
			if (nIndex >= (int) m_CurQueryRes[m_nCurTable].num_fields())
			{
				return "";
			}

			return m_CurQueryRes[m_nCurTable].field_type(nIndex).base_type().name();
		}

		LPCSTR CMYSQLRecordSet::GetFieldValue(int nIndex, TypeAdapter<LPCSTR>)
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

		mysqlpp::DateTime CMYSQLRecordSet::GetFieldValue(int nIndex, TypeAdapter<mysqlpp::DateTime>)
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

		LPCSTR CMYSQLRecordSet::GetFieldValue(const char* pszFieldName, TypeAdapter<LPCSTR>)
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

		mysqlpp::DateTime CMYSQLRecordSet::GetFieldValue(const char* pszFieldName, TypeAdapter<mysqlpp::DateTime>)
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

		BOOL CMYSQLRecordSet::NextRecordset()
		{
			if (m_nCurTable >= (int) m_CurQueryRes.size())
			{
				return FALSE;
			}

			m_nCurTable++;
			m_nCurRow = 0;

			return TRUE;
		}

		UINT CMYSQLRecordSet::GetFieldValueBinary(const char* pszFieldName, char* pszOut, UINT nMaxLen)
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

		const mysqlpp::String& CMYSQLRecordSet::GetFieldString(int nIndex) const
		{
			return m_CurQueryRes[m_nCurTable][m_nCurRow][nIndex];
		}

		const mysqlpp::String& CMYSQLRecordSet::GetFieldString(const char* pszFieldName) const
		{
			return m_CurQueryRes[m_nCurTable][m_nCurRow][pszFieldName];
		}

		bool CMYSQLRecordSet::IsNull(const mysqlpp::String& strField) const
		{
			if (strField.is_null())
			{
				mysqlpp::mysql_type_info mysql_type = strField.type();
				const char* pszType = mysql_type.sql_name();
				if (strstr(pszType, "NULL"))
				{
					return true;
				}
			}

			return false;
		}
	}
}