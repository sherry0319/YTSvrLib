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
// ini配置文件读取器.为了跨平台不能使用GetPrivateProfileSection家族函数

#define SERVER_CONFIG_FILE TEXT("ServerConf.ini")

namespace YTSvrLib
{
	class CConfigLoader
	{
		char m_strFileName[MAX_PATH];
		char m_strSectionName[MAX_PATH];
		BOOL m_bCfgFileOK;
		std::map<std::string, std::string> m_mapConfigKey;
	public:

		CConfigLoader(const char* pFileName)
		{
			m_bCfgFileOK = FALSE;
			memset(m_strFileName, 0, sizeof(m_strFileName));
			SetFile(pFileName);
			memset(m_strSectionName, 0, sizeof(m_strSectionName));
		}

		// 设置读取的文件
		void SetFile(const char* pFileName);

		// 文件是否已经准备完毕
		BOOL IsFileOK();

		// 设置段落[]
		void SetSection(const char* pSectionName);

		// 读取一个数字
		int ParseInt(const char* pKeyName, int nDefualValue = 0);

		// 读取一个字符串
		const char* ParseString(const char* pKeyName, const char* pDefualValue = "");
	};
}