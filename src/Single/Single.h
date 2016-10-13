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
	template<typename T>
	class CSingle
	{
	private:
		static T* m_pInstance;
	public:
		static T* GetInstance()
		{
			if (m_pInstance)
			{
				return m_pInstance;
			}
			m_pInstance = new T;
			return m_pInstance;
		}
		static void DelInstance()
		{
			if (m_pInstance)
			{
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}

	protected:
		CSingle(void)
		{};
		~CSingle(void)
		{};
	private:  // emphasize the following members are private
		CSingle(const CSingle&);
		CSingle& operator=(const CSingle&);
	};

	template<typename T>
	T* CSingle<T>::m_pInstance = NULL;
}