#pragma once
#include "../stl/wqueue.h"

#define ALLOCBUF_SIZE_MAX		(1024<<6)	//64K
struct sAllocBuf
{
	UINT m_nBufSize;
	char* m_pBuf;

	sAllocBuf() {
		m_nBufSize = 0;
		m_pBuf = NULL;
	}
	~sAllocBuf() {
		if( m_pBuf && m_nBufSize > 0 )
			delete [] m_pBuf;
		m_pBuf = NULL;
		m_nBufSize = 0;
	}
};

class CAllocBuf
{
public:
	CAllocBuf(void);
	~CAllocBuf(void);

	static CAllocBuf* m_pInstance;
public:   

	static CAllocBuf* GetInstance()
	{
		if( m_pInstance )
		{
			return m_pInstance;
		}
		m_pInstance = new CAllocBuf;
		return m_pInstance;
	}
	static void DelInstance()
	{
		if( m_pInstance )
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}
	sAllocBuf* Allocate( UINT nBufSize );
	void Release( sAllocBuf* pBuf );

private:
	CWQueue<sAllocBuf*> m_queueIdleBufs;
};
