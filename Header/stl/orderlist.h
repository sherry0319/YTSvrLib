#pragma once
#include "../Global/GlobalServer.h"
class COrderItem
{
public:
	COrderItem(){}

	virtual void SetRank( int nRankd ) {nRankd = 0;};
	virtual int GetRank() = 0;
	virtual int GetID() = 0;
	virtual int GetValue() = 0;
// 	virtual	bool operator > (const COrderItem& b) = 0;
// 	virtual	bool operator == (const COrderItem& b) = 0;
// 	virtual COrderItem & operator = ( const COrderItem & t ) =0;
// 	virtual void Init(){};
// 	virtual void Clear() {};
// 
// 	bool InUse() const          {return m_bInUse;}
// 	void SetInUse(bool bInUse)  {m_bInUse = bInUse;}
// 
// 	void        SetIdx( const std::pair< INT_PTR,INT_PTR >& nIndex )  { m_Idx = nIndex; }
// 
// 	const std::pair< INT_PTR,INT_PTR >& GetIdx(){ return m_Idx; }
};


template<typename ObjectType, long TOPSIZE, long RANKSIZE >
class COrderList
{
public:
	COrderList() {
		//m_ay = new ObjectType[RANKSIZE+1];
		m_nCount = 0;
		DWORD dw = GetPrimeNumInRange(RANKSIZE, RANKSIZE<<1);
		m_map.InitHashTable(dw);
	}
	~COrderList(){
		//delete[] m_ay;
	};
	void Clear()
	{
		m_nCount = 0;
		m_map.RemoveAll();
	}
	void AddNewObj( ObjectType& obj ){
		UINT n = RANKSIZE;
		if( m_map.Lookup( obj, n ) == FALSE 
			|| n >= RANKSIZE )
		{
			if( m_nCount <= 0 )
			{
				m_ay[0] = obj;
				m_ay[0].SetRank( 1 );
				m_nCount = 1;
				m_map.SetAt( obj, 0 );
				return;
			}
			if( m_nCount >= RANKSIZE )
				n = RANKSIZE;
			else
			{
				n = m_nCount;
				m_nCount++;
			}
			m_ay[n] = obj;
			if( n >= TOPSIZE )
				m_ay[n].SetRank( -1 );
			else
				m_ay[n].SetRank( n+1 );
			if( n < RANKSIZE )
				m_map.SetAt( obj, n );
		}
		ReOrder(n);
	}
	void ReOrder( UINT n )
	{
		if( n > RANKSIZE )
			return;
		while( n > 0 && m_ay[n] > m_ay[n-1] )
		{
			ObjectType tmp = m_ay[n];
			m_ay[n] = m_ay[n-1];
			int r = m_ay[n-1].GetRank()+1;
			m_ay[n].SetRank( r );
			if( n < RANKSIZE )
				m_map.SetAt( m_ay[n], n );
			else
				m_map.RemoveKey( m_ay[n] );
			UINT m = n;
			while( m+1 < m_nCount && (m_ay[m] > m_ay[m+1]) == false
				&& (m_ay[m+1] > m_ay[m]) == false )
			{
				m_ay[m+1].SetRank( r );
				m++;
			}
			n--;
			while( n > 0 && (m_ay[n] > m_ay[n-1]) == false
					&& (m_ay[n-1] > m_ay[n]) == false )
			{
				m_ay[n] = m_ay[n-1];
				m_ay[n].SetRank( r );
				if( n < RANKSIZE )
					m_map.SetAt( m_ay[n], n );
				else
					m_map.RemoveKey( m_ay[n] );
				n--;
			}
			m_ay[n] = tmp;
			m_ay[n].SetRank( n+1 );
			m_map.SetAt( m_ay[n], n );
		}
		if( n > 0 && n < RANKSIZE )
		{	
			if(  (m_ay[n] > m_ay[n-1]) == false
				&& (m_ay[n-1] > m_ay[n]) == false )
				m_ay[n].SetRank( m_ay[n-1].GetRank() );
		}
		while( n+1 < m_nCount && n<RANKSIZE && m_ay[n+1] > m_ay[n] )
		{
			ObjectType tmp = m_ay[n];
			m_ay[n] = m_ay[n+1];
			int r = tmp.GetRank();
			if( r < m_ay[n+1].GetRank() )
				r = m_ay[n+1].GetRank()-1;
			m_ay[n].SetRank( r );
			if( n < RANKSIZE )
				m_map.SetAt( m_ay[n], n );
			else
				m_map.RemoveKey( m_ay[n] );
			n++;
			while( n+1 < m_nCount && (m_ay[n] > m_ay[n+1]) == false
				&& (m_ay[n+1] > m_ay[n]) == false )
			{
				m_ay[n] = m_ay[n+1];
				m_ay[n].SetRank( r );
				if( n < RANKSIZE )
					m_map.SetAt( m_ay[n], n );
				else
					m_map.RemoveKey( m_ay[n] );
				n++;
			}
			m_ay[n] = tmp;
			m_ay[n].SetRank( n+1 );
			if( n < RANKSIZE )
				m_map.SetAt( m_ay[n], n );
			else
				m_map.RemoveKey( m_ay[n] );
		}
		while( n+1 < m_nCount && n<RANKSIZE && (m_ay[n] > m_ay[n+1]) == false
					&& (m_ay[n+1] > m_ay[n]) == false )
		{
			m_ay[n+1].SetRank( m_ay[n].GetRank());
			n++;
		}
		return;
	}
	void OnObjChanged( ObjectType& obj )
	{
		UINT n = RANKSIZE;
		if( m_map.Lookup( obj, n ) == FALSE 
			|| n >= RANKSIZE )
		{
			AddNewObj(obj);
			return;
		}
		ReOrder(n);
	}
	void OnObjDeleted( ObjectType& obj )
	{
		UINT n = RANKSIZE;
		if( m_map.Lookup( obj, n ) == FALSE 
			|| n >= RANKSIZE )
			return;
		m_map.RemoveKey( obj );
		int r0 = m_ay[n].GetRank();
		while( n+1 < m_nCount )
		{
			m_ay[n] = m_ay[n+1];
			int r = m_ay[n+1].GetRank();
			if( r > r0 )
				m_ay[n].SetRank( r-1 );
			else
				m_ay[n].SetRank(r);

			if( n < RANKSIZE )
				m_map.SetAt( m_ay[n], n );
			else
				m_map.RemoveKey( m_ay[n] );
			n++;
		}
		if( m_nCount > 0 )
			m_nCount--;
	}
	int GetPosInTop( ObjectType& obj )
	{
		UINT n = RANKSIZE;
		m_map.Lookup( obj, n );
		if( n < TOPSIZE )
			return (int)n;
		return -1;
	}
	ObjectType& GetItem( UINT n )
	{
		if( n < m_nCount && n < RANKSIZE )
			return m_ay[n];
		return m_ay[RANKSIZE];
	}
	void SetItem( ObjectType& obj, int nRank )
	{
		if( nRank <= 0 || nRank > RANKSIZE )
			return;
		UINT n = nRank - 1;
		m_ay[n] = obj;
		m_map.SetAt( obj, n );
		if( nRank == RANKSIZE )
			m_nCount = RANKSIZE;
		else if( n >= m_nCount )
			m_nCount = n+1;
	}
	int GetTopCount()
	{
		if( m_nCount >= TOPSIZE )
			return TOPSIZE;
		return m_nCount;
	}
	void PrintAll()
	{
		for( UINT i=0; i<m_nCount; i++ )
			LOG("ID=%d Value=%d Rank=%d", m_ay[i].GetID(), m_ay[i].GetValue(), m_ay[i].GetRank());
	}
private:
	ObjectType m_ay[RANKSIZE+1];
	CMap<ObjectType, ObjectType&, UINT, UINT> m_map;
	UINT m_nCount;
};