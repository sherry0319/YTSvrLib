#pragma once
//#include "../lib/Service/Log.h"


class CInStream
{
    const char* m_pBuf;
    long m_nLen;
    long m_nCurrentLen;

private:

    template<typename T>
    CInStream& Read( T& Param )
    {
        if( m_nCurrentLen + (long)sizeof(T) > m_nLen )
        {
            LOG( "Buffer Read Error!" );
            return *this;
        }

        Param = *(T*)m_pBuf;
        m_pBuf += sizeof( T );
        m_nCurrentLen += sizeof( T );
        return *this;
    }

public:

    CInStream( const char* pBuf, long nLen ):m_pBuf(pBuf),m_nLen(nLen),m_nCurrentLen(0)
    {}    

    void operator>>( const char*& pChar ){ pChar = m_pBuf; /*return *this;*/}
    //void operator>>( char*& pChar ){ pChar = m_pBuf; /*return *this;*/}
    CInStream& operator>>( char& Param ){ return Read( Param ); }
    CInStream& operator>>( DWORD& Param ){ return Read( Param ); }
    //CInStream& operator>>( BYTE& Param ){ return Read( Param ); }
    //CInStream& operator>>( WORD& Param ){ return Read( Param ); }
    CInStream& operator>>( unsigned char& Param ){ return Read<unsigned char>( Param ); }
    CInStream& operator>>( short& Param ){ return Read<short>( Param ); }
    CInStream& operator>>( unsigned short& Param ){ return Read<unsigned short>( Param ); }
    CInStream& operator>>( long& Param ){ return Read<long>( Param ); }
    CInStream& operator>>( int& Param ){ return Read<int>( Param ); }
    CInStream& operator>>( void*& Param ){ return Read<void*>( Param ); }
	CInStream& operator>>( LONGLONG& Param ){ return Read<LONGLONG>( Param ); }

    CInStream& Skip( long nLen )
    { 
        if( m_nCurrentLen + nLen > m_nLen )
        {
            LOG( "Buffer Skip Error!" );
            return *this;
        }
        m_pBuf += nLen;
        return *this;
    }

    long GetBytesLeft()
    {
        return m_nLen - m_nCurrentLen;
    }

    const char* GetOrgBuf(){ return m_pBuf; }
};

class COutStream
{
    char* m_pBuf;
    long m_nLen;
    long m_nCurrentLen;

    template<typename T>
    COutStream& Write( T Param )
    {
        if( m_nCurrentLen + (long)sizeof(T) > m_nLen )
        {
            LOG( "Buffer Write Error!" );
            return *this;
        }

        *(T*)m_pBuf = Param;
        m_pBuf += sizeof( T );
        m_nCurrentLen += sizeof( T );
        return *this;
    }

public:
    COutStream( char* pBuf, long nLen ):m_pBuf(pBuf), m_nLen(nLen),m_nCurrentLen(0)
    {}

    COutStream& operator<<( const char* Param ){ return Write<const char*>( Param ); }
    COutStream& operator<<( unsigned char Param ){ return Write<unsigned char>( Param ); }
    COutStream& operator<<( short Param ){ return Write<short>( Param ); }
    COutStream& operator<<( long Param ){ return Write<long>( Param ); }
    COutStream& operator<<( int Param ){ return Write<int>( Param ); }
    COutStream& operator<<( void* Param ){ return Write<void*>( Param ); }
};