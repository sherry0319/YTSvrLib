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

namespace YTSvrLib
{
	class CKeywordFilterW : public CSingle<CKeywordFilterW>
	{
	public:
		CKeywordFilterW()
		{

		}
		virtual ~CKeywordFilterW()
		{
			m_vctKeyword.clear();
			m_mpKeyword.clear();
		}
	public:
		void Init()
		{
			m_vctKeyword.clear();
			m_mpKeyword.clear();
		}

		void AddKeyWord(wchar_t* pwzKeyWord);
		void Preprocess();

		void DoFilter(wchar_t* pstrContent, long nTextLen);
		BOOL HasKeyWord(const wchar_t* pstrContent, long nTextLen, wchar_t* pstrOutKey, long nOutMaxLength);

	private:
		std::vector< std::wstring > m_vctKeyword; //Sorted
		typedef std::pair< std::vector< std::wstring >::iterator, std::vector< std::wstring >::iterator > ITPAIR;

		std::map< wchar_t, ITPAIR> m_mpKeyword;
	};

	class CKeywordFilterA : public CSingle < CKeywordFilterA >
	{
	public:
		CKeywordFilterA()
		{

		}
		virtual ~CKeywordFilterA()
		{
			m_vctKeyword.clear();
			m_mpKeyword.clear();
		}
	public:
		void Init()
		{
			m_vctKeyword.clear();
			m_mpKeyword.clear();
		}

		void AddKeyWord(char* pszKeyWord);
		void Preprocess();

		void DoFilter(char* pstrContent, long nTextLen);
		BOOL HasKeyWord(const char* pstrContent, long nTextLen, char* pstrOutKey, long nOutMaxLength);

	private:
		std::vector< std::string > m_vctKeyword; //Sorted
		typedef std::pair< std::vector< std::string >::iterator, std::vector< std::string >::iterator > ITPAIR;

		std::map< char, ITPAIR> m_mpKeyword;
	};
}