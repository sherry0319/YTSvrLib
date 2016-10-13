/*----------------------------------------------------------------------
"Debugging .NET and Win32/64 Applications" (Microsoft Press)
Copyright (c) 1997-2002 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/

#ifndef _RESSTRING_H
#define _RESSTRING_H

/*//////////////////////////////////////////////////////////////////////
                           Necessary Includes
//////////////////////////////////////////////////////////////////////*/

/*//////////////////////////////////////////////////////////////////////
                          Constants & Defines
//////////////////////////////////////////////////////////////////////*/

/*//////////////////////////////////////////////////////////////////////
                                Typedefs
//////////////////////////////////////////////////////////////////////*/

/*//////////////////////////////////////////////////////////////////////
                               Prototypes
//////////////////////////////////////////////////////////////////////*/

// A simple class that wraps resource string loading.
class CResString
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    // hInst - The module instance to load strings from.
    CResString ( HINSTANCE hInst = NULL )
    {
        if ( NULL == hInst )
        {
            hInst = GetModuleHandle ( NULL ) ;
        }
        m_hInst = hInst ;
        m_szBuff[ 0 ] = '\0' ;
    }

    CResString ( HINSTANCE hInst , UINT uiRes )
    {
        ASSERT ( NULL != hInst ) ;
        m_hInst = hInst ;
        VERIFY ( LoadString ( uiRes ) ) ;
    }

    virtual ~CResString ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                 Public Member Functions and Operators
//////////////////////////////////////////////////////////////////////*/
public      :
    LPCTSTR LoadString ( UINT uiRes )
    {
        ASSERT ( NULL != m_hInst ) ;
        if ( 0 == ::LoadString ( m_hInst     ,
                                 uiRes       ,
                                 m_szBuff    ,
                                 sizeof ( m_szBuff ) / sizeof ( TCHAR)))
        {
            return ( NULL ) ;
        }
        return ( m_szBuff ) ;
    }

    operator LPCTSTR ( ) const
    {
        return ( m_szBuff ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                          Private Data Members
//////////////////////////////////////////////////////////////////////*/
private     :

    // The module instance.
    HINSTANCE m_hInst ;
    // The string space.
    TCHAR m_szBuff[ MAX_PATH ] ;
} ;

#endif  // _RESSTRING_H


