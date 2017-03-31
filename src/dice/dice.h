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

#include "../Service/Utility.h"
// ÷»×ÓÀà.
namespace YTSvrLib
{
	template<typename _TYPE>
	class CDice
	{
	public:
		struct DiceItem
		{
			_TYPE m_Item;
			int m_nRate;
		};
	public:
		CDice()
		{
			m_vctRandomList.clear();
			m_nTotalRate = 0;
		}

		virtual ~CDice()
		{
			m_vctRandomList.clear();
			m_nTotalRate = 0;
		}

	public:
		void AddItem(_TYPE val, int nRate)
		{
			DiceItem sItem;
			sItem.m_Item = val;
			sItem.m_nRate = nRate;

			m_vctRandomList.push_back(sItem);

			m_nTotalRate += nRate;
		}

		_TYPE GetRandom()
		{
			if (m_nTotalRate == 0)
			{
				return (_TYPE) 0;
			}
			int nRand = Random2(m_nTotalRate);

			for (size_t i = 0; i < m_vctRandomList.size(); ++i)
			{
				if (m_vctRandomList[i].m_nRate > nRand)
				{
					return m_vctRandomList[i].m_Item;
				}
				nRand -= m_vctRandomList[i].m_nRate;
			}

			return (_TYPE) 0;
		}
	private:
		std::vector<DiceItem> m_vctRandomList;
		int m_nTotalRate;
	};
}