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
#include "stdafx.h"
#include "AutoCloseFile.h"

namespace YTSvrLib
{
	void Trim(std::string& strSrc)
	{
		size_t nTrimL = 0;
		for (size_t i = 0; i < strSrc.size(); ++i)
		{
			if (strSrc[i] == ' ' || strSrc[i] == '\r' || strSrc[i] == '\t' || strSrc[i] == '\f' || strSrc[i] == '\v')
			{
				nTrimL++;
			}
			else
			{
				break;
			}
		}

		if (nTrimL > 0)
		{
			strSrc.erase(0, nTrimL);
		}

		size_t nTrimR = 0;
		for (size_t i = strSrc.size() - 1; i >= 0; --i)
		{
			if (strSrc[i] == ' ' || strSrc[i] == '\r' || strSrc[i] == '\t' || strSrc[i] == '\f' || strSrc[i] == '\v')
			{
				nTrimR++;
			}
			else
			{
				break;
			}
		}

		if (nTrimR > 0)
		{
			strSrc.erase((strSrc.size() - nTrimR), nTrimR);
		}
	}

	void CConfigLoader::SetFile(const char* pFileName)
	{
		FILE* pFile = fopen(pFileName, "r");
		if (pFile == NULL)
		{
			printf("Could not open file: %s (error %d)\n", pFileName, GetLastError());
			m_bCfgFileOK = FALSE;
			return;
		}

		m_bCfgFileOK = TRUE;

		fseek(pFile, 0, SEEK_END);
		size_t filesize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		char* filebuffer = new char[filesize + 1];
		memset(filebuffer, 0, filesize);

		fread(filebuffer, filesize, 1, pFile);

		std::vector<std::string> vctList;

		if (filesize > 3 && filebuffer[0] == '\xEF' && filebuffer[1] == '\xBB' && filebuffer[2] == '\xBF')
		{// »•µÙBOMÕ∑
			StrDelimiter(filebuffer+3, "\n", vctList);
		}
		else
		{
			StrDelimiter(filebuffer, "\n", vctList);
		}

		delete[] filebuffer;
		filebuffer = NULL;

		fclose(pFile);
		pFile = NULL;

		std::string strCurSection;

		m_mapConfigKey.clear();

		for (size_t i = 0; i < vctList.size(); ++i)
		{
			if (vctList[i].size() > 0)
			{
				if (vctList[i][0] == '[')
				{
					Trim(vctList[i]);
					strCurSection = vctList[i];
					// printf_s("Cur Section : %s\n", strCurSection.c_str());
				}
				else if (vctList[i][0] != ';' && vctList[i][0] != '#'&& vctList[i][0] != '\r' && vctList[i][0] != '\n')
				{
					std::vector<std::string> vctItem;
					StrDelimiter(vctList[i], "=", vctItem);

					if (vctItem.size() >= 2)
					{
						Trim(vctItem[0]);
						Trim(vctItem[1]);

						std::string strKey = strCurSection + vctItem[0];

						m_mapConfigKey[strKey] = vctItem[1];

						// printf_s("Config Read : %s = %s\n", strKey.c_str(), vctItem[1].c_str());
					}
				}
			}
		}

		strncpy_s(m_strFileName, pFileName, _countof(m_strFileName));
		memset(m_strSectionName, 0, sizeof(m_strSectionName));
	}

	BOOL CConfigLoader::IsFileOK()
	{
		return m_bCfgFileOK;
	}

	void CConfigLoader::SetSection(const char* pSectionName)
	{
		strncpy_s(m_strSectionName, pSectionName, _countof(m_strSectionName));
	}

	int CConfigLoader::ParseInt(const char* pKeyName, int nDefualValue /*= 0*/)
	{
		const char* pszOut = ParseString(pKeyName, "");
		if (strlen(pszOut) == 0)
		{
			return nDefualValue;
		}
		return atoi(pszOut);
	}

	const char* CConfigLoader::ParseString(const char* pKeyName, const char* pDefualValue /*= ""*/)
	{
		std::string strKey = "[";
		strKey += m_strSectionName;
		strKey += "]";
		strKey += pKeyName;

		if (m_mapConfigKey.find(strKey) == m_mapConfigKey.end())
		{
			return pDefualValue;
		}

		return m_mapConfigKey[strKey].c_str();
	}
}