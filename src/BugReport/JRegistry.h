/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JREGISTRY_H
#define _JREGISTRY_H

class JRegistry
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JRegistry ( void )
    {
        m_hKey = NULL ;
    }

    virtual ~JRegistry ( void )
    {
        Close ( ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                       Close, Open & Create Keys
//////////////////////////////////////////////////////////////////////*/
public      :

    // Closes an opened key.
    void Close ( void )
    {
        if ( NULL != m_hKey )
        {
#ifdef _DEBUG
            LONG lRet =
#endif
                        RegCloseKey ( m_hKey ) ;
#ifdef _DEBUG
            ASSERT ( ERROR_SUCCESS == lRet ) ;
#endif
            m_hKey = NULL ;
        }
    }

    BOOL OpenKey ( HKEY    hOpenKey ,
                   LPCTSTR szSubKey  )
    {
        // Protect against leaks.
        ASSERT ( NULL == m_hKey ) ;
        Close ( ) ;
        LONG lRet = ::RegOpenKeyEx ( hOpenKey           ,
                                     szSubKey           ,
                                     0                  ,
                                     KEY_QUERY_VALUE |
                                        KEY_SET_VALUE   ,
                                     &m_hKey             ) ;
        return ( ERROR_SUCCESS == lRet ) ;
    }

    BOOL CreateKey ( HKEY    hOpenKey ,
                     LPCTSTR szSubKey ,
                     LPDWORD dwDisp = NULL )
    {
        // Protect against leaks.
        ASSERT ( NULL == m_hKey ) ;
        Close ( ) ;
        LONG lRet = ::RegCreateKeyEx ( hOpenKey             ,
                                       szSubKey             ,
                                       0                    ,
                                       NULL                 ,
                                       0                    ,
                                       KEY_QUERY_VALUE |
                                         KEY_SET_VALUE      ,
                                       NULL                 ,
                                       &m_hKey              ,
                                       (NULL == dwDisp)
                                        ? NULL
                                        : dwDisp             ) ;
        return ( ERROR_SUCCESS == lRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                             Query and Sets
//////////////////////////////////////////////////////////////////////*/
    BOOL QueryValue ( LPCTSTR szName ,
                      BOOL &  bVal    )
    {
        return ( QueryValue ( szName , (DWORD&)bVal ) ) ;
    }

    BOOL QueryValue ( LPCTSTR szName ,
                      DWORD & dwVal   )
    {
        ASSERT ( NULL != m_hKey ) ;
        DWORD dwType = REG_DWORD ;
        DWORD dwSize = sizeof ( DWORD ) ;
        LONG lRet = ::RegQueryValueEx ( m_hKey          ,
                                        szName          ,
                                        NULL            ,
                                        &dwType         ,
                                        (LPBYTE)&dwVal  ,
                                        &dwSize          ) ;
        return ( ERROR_SUCCESS == lRet ) ;
    }

    BOOL QueryValue ( LPCTSTR szName    ,
                      LPTSTR  szVal     ,
                      DWORD   dwValSize  )
    {
        ASSERT ( NULL != m_hKey ) ;
        DWORD dwType = REG_SZ ;
        DWORD dwSize = dwValSize ;
        LONG lRet = ::RegQueryValueEx ( m_hKey        ,
                                        szName        ,
                                        NULL          ,
                                        &dwType       ,
                                        (LPBYTE)szVal ,
                                        &dwSize        ) ;
        return ( ERROR_SUCCESS == lRet ) ;
    }

    BOOL QueryValue ( LPCTSTR szName     ,
                      LPVOID  pData      ,
                      DWORD   dwDataSize  )
    {
        ASSERT ( NULL != m_hKey ) ;
        DWORD dwType = REG_BINARY ;
        DWORD dwSize = dwDataSize ;
        LONG lRet = ::RegQueryValueEx ( m_hKey          ,
                                        szName          ,
                                        NULL            ,
                                        &dwType         ,
                                        (LPBYTE)pData   ,
                                        &dwSize          ) ;
        return ( ERROR_SUCCESS == lRet ) ;
    }

    BOOL SetValue ( LPCTSTR szName  ,
                    BOOL    bData    )
    {
        return ( SetValue ( szName , (DWORD)bData ) ) ;
    }

    BOOL SetValue ( LPCTSTR szName  ,
                    DWORD   dwData   )
    {
        ASSERT ( NULL != m_hKey ) ;
        LONG lRet = ::RegSetValueEx ( m_hKey                ,
                                      szName                ,
                                      0                     ,
                                      REG_DWORD             ,
                                      (CONST BYTE *)&dwData ,
                                      sizeof ( DWORD )       ) ;
        ASSERT ( ERROR_SUCCESS == lRet ) ;
        return ( ERROR_SUCCESS == lRet ) ;
    }

    BOOL SetValue ( LPCTSTR szName  ,
                    LPCTSTR szData  ,
                    DWORD   dwSize   )
    {
        ASSERT ( NULL != m_hKey ) ;
        LONG lRet = ::RegSetValueEx ( m_hKey                ,
                                      szName                ,
                                      0                     ,
                                      REG_SZ                ,
                                      (CONST BYTE *)szData  ,
                                      dwSize                 ) ;
        ASSERT ( ERROR_SUCCESS == lRet ) ;
        return ( ERROR_SUCCESS == lRet ) ;
    }

    BOOL SetValue ( LPCTSTR szName  ,
                    LPVOID  pData  ,
                    DWORD   dwSize   )
    {
        ASSERT ( NULL != m_hKey ) ;
        LONG lRet = ::RegSetValueEx ( m_hKey                ,
                                      szName                ,
                                      0                     ,
                                      REG_BINARY            ,
                                      (CONST BYTE *)pData   ,
                                      dwSize                 ) ;
        ASSERT ( ERROR_SUCCESS == lRet ) ;
        return ( ERROR_SUCCESS == lRet ) ;
    }



/*//////////////////////////////////////////////////////////////////////
                          Private Member Data
//////////////////////////////////////////////////////////////////////*/
private     :

    // The registry key opened by this class.
    HKEY m_hKey ;
} ;

#endif  // _JREGISTRY_H


