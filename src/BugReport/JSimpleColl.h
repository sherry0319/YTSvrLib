// I lifted this from the ATL source code as I wanted to use these
// classes, but did not want to be forced to link against ATL.
// The only difference is that I renamed the classes.
// Note that this file has NO dependencies on JFX proper so you can use
// it by itself.

// This is a part of the Active Template Library.
// Copyright (C) 1996-2001 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Active Template Library Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Active Template Library product.

#ifndef _JSIMPLECOLL_H
#define _JSIMPLECOLL_H

#pragma once

#pragma push_macro("malloc")
#undef malloc
#pragma push_macro("realloc")
#undef realloc
#pragma push_macro("free")
#undef free

#pragma push_macro("new")
#undef new

/////////////////////////////////////////////////////////////////////////////
// Collection helpers - JSimpleArray & JSimpleMap

// template class helpers with functions for comparing elements
// override if using complex types without operator==
template <class T>
class JSimpleArrayEqualHelper
{
public:
	static bool IsEqual(const T& t1, const T& t2)
	{
		return (t1 == t2);
	}
};

template <class T>
class JSimpleArrayEqualHelperFalse
{
public:
	static bool IsEqual(const T&, const T&)
	{
		ASSERT(false);
		return false;
	}
};

template <class TKey, class TVal>
class JSimpleMapEqualHelper
{
public:
	static bool IsEqualKey(const TKey& k1, const TKey& k2)
	{
		return JSimpleArrayEqualHelper<TKey>::IsEqual(k1, k2);
	}

	static bool IsEqualValue(const TVal& v1, const TVal& v2)
	{
		return JSimpleArrayEqualHelper<TVal>::IsEqual(v1, v2);
	}
};

template <class TKey, class TVal>
class JSimpleMapEqualHelperFalse
{
public:
	static bool IsEqualKey(const TKey& k1, const TKey& k2)
	{
		return JSimpleArrayEqualHelper<TKey>::IsEqual(k1, k2);
	}

	static bool IsEqualValue(const TVal&, const TVal&)
	{
		ASSERT(FALSE);
		return false;
	}
};

template <class T, class TEqual = JSimpleArrayEqualHelper< T > >
class JSimpleArray
{
public:
// Construction/destruction
	JSimpleArray() : m_aT(NULL), m_nSize(0), m_nAllocSize(0)
	{ }

	~JSimpleArray()
	{
		RemoveAll();
	}

	JSimpleArray(const JSimpleArray< T, TEqual >& src) : m_aT(NULL), m_nSize(0), m_nAllocSize(0)
	{
		m_aT = (T*)malloc(src.GetSize() * sizeof(T));
		if (m_aT != NULL)
		{
			m_nAllocSize = src.GetSize();
			for (int i=0; i<src.GetSize(); i++)
				Add(src[i]);
		}
	}
	JSimpleArray< T, TEqual >& operator=(const JSimpleArray< T, TEqual >& src)
	{
		if (GetSize() != src.GetSize())
		{
			RemoveAll();
			m_aT = (T*)malloc(src.GetSize() * sizeof(T));
			if (m_aT != NULL)
				m_nAllocSize = src.GetSize();
		}
		else
		{
			for (int i = GetSize(); i > 0; i--)
				RemoveAt(i - 1);
		}
		for (int i=0; i<src.GetSize(); i++)
			Add(src[i]);
		return *this;
	}

// Operations
	int GetSize() const
	{
		return m_nSize;
	}
	BOOL Add(const T& t)
	{
		if(m_nSize == m_nAllocSize)
		{
			T* aT;
			int nNewAllocSize = (m_nAllocSize == 0) ? 1 : (m_nSize * 2);
			aT = (T*)realloc(m_aT, nNewAllocSize * sizeof(T));
			if(aT == NULL)
				return FALSE;
			m_nAllocSize = nNewAllocSize;
			m_aT = aT;
		}
		m_nSize++;
		InternalSetAtIndex(m_nSize - 1, t);
		return TRUE;
	}
	BOOL Remove(const T& t)
	{
		int nIndex = Find(t);
		if(nIndex == -1)
			return FALSE;
		return RemoveAt(nIndex);
	}
	BOOL RemoveAt(int nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		if (nIndex < 0 || nIndex >= m_nSize)
			return FALSE;
		m_aT[nIndex].~T();
		if(nIndex != (m_nSize - 1))
			memmove((void*)(m_aT + nIndex), (void*)(m_aT + nIndex + 1), (m_nSize - (nIndex + 1)) * sizeof(T));
		m_nSize--;
		return TRUE;
	}
	void RemoveAll()
	{
		if(m_aT != NULL)
		{
			for(int i = 0; i < m_nSize; i++)
				m_aT[i].~T();
			free(m_aT);
			m_aT = NULL;
		}
		m_nSize = 0;
		m_nAllocSize = 0;
	}
	const T& operator[] (int nIndex) const
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_aT[nIndex];
	}
	T& operator[] (int nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_aT[nIndex];
	}
	T* GetData() const
	{
		return m_aT;
	}

	int Find(const T& t) const
	{
		for(int i = 0; i < m_nSize; i++)
		{
			if(TEqual::IsEqual(m_aT[i], t))
				return i;
		}
		return -1;  // not found
	}

	BOOL SetAtIndex(int nIndex, const T& t)
	{
		if (nIndex < 0 || nIndex >= m_nSize)
			return FALSE;
		InternalSetAtIndex(nIndex, t);
		return TRUE;
	}

// Implementation
	class Wrapper
	{
	public:
		Wrapper(const T& _t) : t(_t)
		{
		}
		template <class _Ty>
		void * __cdecl operator new(size_t, _Ty* p)
		{
			return p;
		}
		template <class _Ty>
		void __cdecl operator delete(void* /* pv */, _Ty* /* p */)
		{
		}
		T t;
	};

// Implementation
	void InternalSetAtIndex(int nIndex, const T& t)
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		new(m_aT + nIndex) Wrapper(t);
	}

	typedef T _ArrayElementType;
	T* m_aT;
	int m_nSize;
	int m_nAllocSize;

};

#define JSimpleValArray JSimpleArray

// intended for small number of simple types or pointers
template <class TKey, class TVal, class TEqual = JSimpleMapEqualHelper< TKey, TVal > >
class JSimpleMap
{
public:
	TKey* m_aKey;
	TVal* m_aVal;
	int m_nSize;

	typedef TKey _ArrayKeyType;
	typedef TVal _ArrayElementType;

// Construction/destruction
	JSimpleMap() : m_aKey(NULL), m_aVal(NULL), m_nSize(0)
	{ }

	~JSimpleMap()
	{
		RemoveAll();
	}

// Operations
	int GetSize() const
	{
		return m_nSize;
	}
	BOOL Add(const TKey& key, const TVal& val)
	{
		TKey* pKey;
		pKey = (TKey*)realloc(m_aKey, (m_nSize + 1) * sizeof(TKey));
		if(pKey == NULL)
			return FALSE;
		m_aKey = pKey;
		TVal* pVal;
		pVal = (TVal*)realloc(m_aVal, (m_nSize + 1) * sizeof(TVal));
		if(pVal == NULL)
			return FALSE;
		m_aVal = pVal;
		m_nSize++;
		InternalSetAtIndex(m_nSize - 1, key, val);
		return TRUE;
	}
	BOOL Remove(const TKey& key)
	{
		int nIndex = FindKey(key);
		if(nIndex == -1)
			return FALSE;
		return RemoveAt(nIndex);
	}
	BOOL RemoveAt(int nIndex)
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		if (nIndex < 0 || nIndex >= m_nSize)
			return FALSE;
		m_aKey[nIndex].~TKey();
		m_aVal[nIndex].~TVal();
		if(nIndex != (m_nSize - 1))
		{
			memmove((void*)(m_aKey + nIndex), (void*)(m_aKey + nIndex + 1), (m_nSize - (nIndex + 1)) * sizeof(TKey));
			memmove((void*)(m_aVal + nIndex), (void*)(m_aVal + nIndex + 1), (m_nSize - (nIndex + 1)) * sizeof(TVal));
		}
		TKey* pKey;
		pKey = (TKey*)realloc(m_aKey, (m_nSize - 1) * sizeof(TKey));
		if(pKey != NULL || m_nSize == 1)
			m_aKey = pKey;
		TVal* pVal;
		pVal = (TVal*)realloc(m_aVal, (m_nSize - 1) * sizeof(TVal));
		if(pVal != NULL || m_nSize == 1)
			m_aVal = pVal;
		m_nSize--;
		return TRUE;
	}
	void RemoveAll()
	{
		if(m_aKey != NULL)
		{
			for(int i = 0; i < m_nSize; i++)
			{
				m_aKey[i].~TKey();
				m_aVal[i].~TVal();
			}
			free(m_aKey);
			m_aKey = NULL;
		}
		if(m_aVal != NULL)
		{
			free(m_aVal);
			m_aVal = NULL;
		}

		m_nSize = 0;
	}
	BOOL SetAt(const TKey& key, const TVal& val)
	{
		int nIndex = FindKey(key);
		if(nIndex == -1)
			return FALSE;
		m_aKey[nIndex].~TKey();
		m_aVal[nIndex].~TVal();
		InternalSetAtIndex(nIndex, key, val);
		return TRUE;
	}
	TVal Lookup(const TKey& key) const
	{
		int nIndex = FindKey(key);
		if(nIndex == -1)
			return NULL;    // must be able to convert
		return GetValueAt(nIndex);
	}
	TKey ReverseLookup(const TVal& val) const
	{
		int nIndex = FindVal(val);
		if(nIndex == -1)
			return NULL;    // must be able to convert
		return GetKeyAt(nIndex);
	}
	TKey& GetKeyAt(int nIndex) const
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_aKey[nIndex];
	}
	TVal& GetValueAt(int nIndex) const
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return m_aVal[nIndex];
	}

	int FindKey(const TKey& key) const
	{
		for(int i = 0; i < m_nSize; i++)
		{
			if(TEqual::IsEqualKey(m_aKey[i], key))
				return i;
		}
		return -1;  // not found
	}
	int FindVal(const TVal& val) const
	{
		for(int i = 0; i < m_nSize; i++)
		{
			if(TEqual::IsEqualValue(m_aVal[i], val))
				return i;
		}
		return -1;  // not found
	}

	BOOL SetAtIndex(int nIndex, const TKey& key, const TVal& val)
	{
		if (nIndex < 0 || nIndex >= m_nSize)
			return FALSE;
		InternalSetAtIndex(nIndex, key, val);
		return TRUE;
	}


// Implementation

	template <typename T>
	class Wrapper
	{
	public:
		Wrapper(const T& _t) : t(_t)
		{
		}
		template <class _Ty>
		void *operator new(size_t, _Ty* p)
		{
			return p;
		}
		template <class _Ty>
		void operator delete(void* /* pv */, _Ty* /* p */)
		{
		}
		T t;
	};
	void InternalSetAtIndex(int nIndex, const TKey& key, const TVal& val)
	{
		ASSERT(nIndex >= 0 && nIndex < m_nSize);
		new(m_aKey + nIndex) Wrapper<TKey>(key);
		new(m_aVal + nIndex) Wrapper<TVal>(val);
	}
};

#pragma pop_macro("new")

#pragma pop_macro("free")
#pragma pop_macro("realloc")
#pragma pop_macro("malloc")


#endif  // _JSIMPLECOLL_H
