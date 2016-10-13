#ifndef _MY_RANDOM_H_
#define _MY_RANDOM_H_

#include <algorithm>
#include "../Service/Utility.h"

template< UINT RANGE, UINT RATIO >
class CMyRandom
{
public:

	CMyRandom()
	{
		Init();
	}
	~CMyRandom()
	{
		m_vctRands.clear();
	}

	void Init()
	{
		m_vctRands.resize(RANGE);
		for (UINT i = 0; i < RANGE; i++)
			m_vctRands[i] = i;
#ifdef LIB_WINDOWS
		srand(GetTickCount() + (Random2(10000) << 15));
#else
		srand(time(NULL) + (Random2(10000) << 15));
#endif // LIB_WINDOWS
		random_shuffle(m_vctRands.begin(), m_vctRands.end());
		random_shuffle(m_vctRands.begin(), m_vctRands.end());
		random_shuffle(m_vctRands.begin(), m_vctRands.end());
		m_nIdx = Random2(RANGE);
	}
	UINT GetRandom()
	{
		if (m_nIdx >= m_vctRands.size())
			m_nIdx = 0;
		UINT nRet = m_vctRands[m_nIdx];
		if (++m_nIdx >= m_vctRands.size())
			m_nIdx = 0;
		return nRet;
	}
	UINT GetRandomInRange(UINT nMax, UINT nMin = 0)
	{
		for (UINT i = 0; i < RANGE; i++)
		{
			UINT nRand = GetRandom();
			if (nRand < nMax && nRand >= nMin)
				return nRand;
		}
		return 0;
	}
	BOOL GetResult()
	{
		UINT nRand = GetRandom();
		if (nRand < RATIO)
			return TRUE;
		return FALSE;
	}
	UINT GetRatio()
	{
		return RATIO;
	}

private:

	std::vector<UINT>	m_vctRands;
	UINT m_nIdx;
};


template< typename T, long SIZE = 1>
class CMyRandomArray
{
public:

	CMyRandomArray()
	{ /*if( SIZE > 0 ) m_vct.resize(SIZE);*/
	}
	~CMyRandomArray()
	{
		m_vctRand.clear();
		m_vctTypes.clear();
	}

	void AddItem(T tItem, UINT nRatio)
	{
		for (UINT i = 0; i< nRatio; i++)
			m_vctRand.push_back(tItem);
		m_vctTypes.push_back(tItem);
	}
	void Init()
	{
		if (SIZE > m_vctRand.size())
			m_vctRand.resize(SIZE);
#ifdef LIB_WINDOWS
		srand(GetTickCount() + (Random2(10000) << 15));
#else
		srand(time(NULL) + (Random2(10000) << 15));
#endif // LIB_WINDOWS
		random_shuffle(m_vctRand.begin(), m_vctRand.end());
		random_shuffle(m_vctRand.begin(), m_vctRand.end());
		random_shuffle(m_vctRand.begin(), m_vctRand.end());
		m_nIdx = 0;
	}
	T& GetRandomItem()
	{
		if (m_nIdx >= m_vctRand.size())
			Init();
		return m_vctRand[m_nIdx++];
	}
	size_t GetSize()
	{
		return m_vctRand.size();
	}
	void Clear()
	{
		m_vctRand.clear();
		m_nIdx = 0;
		m_vctTypes.clear();
	}
	size_t GetTypeCount()
	{
		return m_vctTypes.size();
	}
	T& GetType(size_t n)
	{
		if (n >= m_vctTypes.size())
			n = 0;
		return m_vctTypes[n];
	}

private:

	std::vector<T>	m_vctRand;
	std::vector<T>	m_vctTypes;
	UINT m_nIdx;
};


template< typename T, T ERROR_RET, long SIZE = 1>
class CMyRandomArray2
{
public:
	struct sRandomArray2_Item
	{
		T m_Item;
		UINT m_nRatio;
	};
	CMyRandomArray2()
	{
		m_nRandRange = 0; m_err = ERROR_RET; /*if( SIZE > 0 ) m_vct.resize(SIZE);*/
	}
	~CMyRandomArray2()
	{
		m_vctRand.clear();
		m_vctTypes.clear();
	}

	void AddItem(T tItem, UINT nRatio)
	{
		sRandomArray2_Item sItem;
		sItem.m_Item = tItem;
		sItem.m_nRatio = nRatio;
		m_vctRand.push_back(sItem);
		m_nRandRange += nRatio;
		m_vctTypes.push_back(tItem);
	}
	void Init()
	{
		if (SIZE > m_nRandRange)
			m_nRandRange = SIZE;
		m_nIdx = 0;
	}
	T& GetRandomItem()
	{
		UINT nRand = Random2(m_nRandRange);
		for (UINT i = 0; i < m_vctRand.size(); i++)
		{
			if (m_nIdx >= m_vctRand.size())
				m_nIdx = 0;
			if (nRand < m_vctRand[m_nIdx].m_nRatio)
				return m_vctRand[m_nIdx++].m_Item;
			nRand -= m_vctRand[m_nIdx].m_nRatio;
			m_nIdx++;
		}
		return m_err;
	}
	UINT GetSize()
	{
		return m_nRandRange;
	}
	void Clear()
	{
		m_vctRand.clear();
		m_vctTypes.clear();
		m_nIdx = 0;
		m_nRandRange = 0;
	}
	UINT GetTypeCount()
	{
		return m_vctTypes.size();
	}
	T& GetType(UINT n)
	{
		if (n >= m_vctTypes.size())
			n = 0;
		return m_vctTypes[n];
	}

private:

	std::vector<sRandomArray2_Item>	m_vctRand;
	UINT m_nRandRange;
	std::vector<T>	m_vctTypes;
	UINT m_nIdx;
	T m_err;
};

#endif // !_MY_RANDOM_H_