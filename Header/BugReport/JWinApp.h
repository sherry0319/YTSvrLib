/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JWINAPP_H
#define _JWINAPP_H

class JWinApp
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JWinApp ( void )
    {
        m_pTheApp = this ;
        m_pMainWnd = NULL ;
        m_hInstMain = NULL ;
        m_hInstResources = NULL ;
    }

    ~JWinApp ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                     Internationalization Functions
//////////////////////////////////////////////////////////////////////*/
public      :
    // By default, resources will be pulled out of the main EXE.
    // However, to allow full internationalization, call this function
    // in your InitInstance.  If m_hInstMain is not NULL and the
    // resource is not found in the international DLL, that
    // module will be used.
    virtual BOOL LoadInternationalResourcesDLL ( LPCTSTR szPrefix ) ;

    HINSTANCE GetResourceInstance ( void ) const
    {
        return ( m_hInstResources ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                 Initiating and Exiting The Application
//////////////////////////////////////////////////////////////////////*/
public      :
    // Gets the instance handle for where the JWndClass is registered.
    // This method is only called when JFX's WinMain runs so it's only
    // for standalone applications, not DLL based ones.
    virtual HINSTANCE GetInstanceForClassRegistration ( void )
    {
        return ( m_hInstMain ) ;
    }

    // The instance initialization.  Return TRUE to spin in the message
    // loop.  FALSE will skip the message loop.
    virtual BOOL InitInstance ( void )
    {
        return ( FALSE ) ;
    }

    // Allows you to clean up the application.
    virtual BOOL ExitInstance ( void )
    {
        return ( FALSE ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                           HINSTANCE Handlers
//////////////////////////////////////////////////////////////////////*/
public      :
    HCURSOR LoadCursor ( UINT nResourceID )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        return ( LoadCursor ( MAKEINTRESOURCE ( nResourceID ) ) ) ;
    }

    HCURSOR LoadCursor ( LPCTSTR szResource )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        HCURSOR hRes = ::LoadCursor ( m_hInstResources , szResource ) ;
        if ( ( NULL == hRes ) && ( NULL != m_hInstMain ) )
        {
            hRes = ::LoadCursor ( m_hInstMain , szResource ) ;
        }
        return ( hRes ) ;
    }

    HCURSOR LoadStandardCursor ( LPCTSTR szResource )
    {
        return ( ::LoadCursor ( NULL , szResource ) ) ;
    }

    HICON LoadIcon ( UINT nResourceID )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        return ( LoadIcon ( MAKEINTRESOURCE ( nResourceID ) ) ) ;
    }

    HICON LoadIcon ( LPCTSTR szResource )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        HICON hRes = ::LoadIcon ( m_hInstResources , szResource ) ;
        if ( ( NULL == hRes ) && ( NULL != m_hInstMain ) )
        {
            hRes = ::LoadIcon ( m_hInstMain , szResource ) ;
        }
        return ( hRes ) ;
    }

    HICON LoadSmallIcon ( UINT nResourceID )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        return ( LoadSmallIcon ( MAKEINTRESOURCE ( nResourceID ) ) ) ;
    }

    // Remember to call DestroyIcon after using this one!
    HICON LoadSmallIcon ( LPCTSTR szResource )
    {
        HICON hRes =  (HICON)LoadImage ( m_hInstResources    ,
                                         szResource          ,
                                         IMAGE_ICON          ,
                                         16                  ,
                                         16                  ,
                                         LR_DEFAULTCOLOR      ) ;
        if ( ( NULL == hRes ) && ( NULL != m_hInstMain ) )
        {
            hRes = (HICON)LoadImage ( m_hInstMain           ,
                                      szResource            ,
                                      IMAGE_ICON            ,
                                      16                    ,
                                      16                    ,
                                      LR_DEFAULTCOLOR        ) ;
        }
        return ( hRes ) ;
    }


    HICON LoadStandardIcon ( LPCTSTR szResource )
    {
        return ( ::LoadIcon ( NULL , szResource ) ) ;
    }

    HMENU LoadMenu ( UINT nResourceID )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        return ( LoadMenu ( MAKEINTRESOURCE ( m_hInstResources ) ) ) ;
    }

    HMENU LoadMenu ( LPCTSTR szResource )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        HMENU hRes = ::LoadMenu ( m_hInstResources , szResource )  ;
        if ( ( NULL == hRes ) && ( NULL != m_hInstMain ) )
        {
            hRes = ::LoadMenu ( m_hInstMain , szResource ) ;
        }
        return ( hRes ) ;
    }

    LPCDLGTEMPLATE LoadDialog ( UINT        nResourceID          ,
                                HINSTANCE * phInstFoundIn = NULL  )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        return ( LoadDialog ( MAKEINTRESOURCE ( nResourceID ) ,
                              phInstFoundIn                    ) ) ;
    }

    LPCDLGTEMPLATE LoadDialog ( LPCTSTR     szResource           ,
                                HINSTANCE * phInstFoundIn = NULL  ) ;

    int LoadString ( UINT uID , LPTSTR lpBuffer , int nBufferMax )
    {
        ASSERT ( NULL != m_hInstResources ) ;
        int iRes = ::LoadString ( m_hInstResources ,
                                  uID              ,
                                  lpBuffer         ,
                                  nBufferMax        ) ;
        if ( ( 0 == iRes ) && ( NULL != m_hInstMain ) )
        {
            iRes = ::LoadString ( m_hInstMain      ,
                                  uID              ,
                                  lpBuffer         ,
                                  nBufferMax        ) ;
        }
        return ( iRes ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                        Public Method Functions
                  Message Looping and Process Control
//////////////////////////////////////////////////////////////////////*/
    // The main message looper.
    virtual int Run ( void ) ;

/*//////////////////////////////////////////////////////////////////////
                           Public Member Data
//////////////////////////////////////////////////////////////////////*/
public      :
    // The main "window".  It's up to the user to set this.
    JWnd * m_pMainWnd ;
    // The instance handle for the main module.
    HINSTANCE m_hInstMain ;
    // The instance handle for resources.  Derived classes need to
    // set this value if they want default resources to come out of
    // another module.
    HINSTANCE m_hInstResources ;
    // The application name.  It's up to the user to set this.
    JString m_sAppName ;

/*//////////////////////////////////////////////////////////////////////
                       Public Static Member Data
//////////////////////////////////////////////////////////////////////*/
public      :
    // The once application instance.  Don't use this directly, use
    // JfxGetApp instead.
    static JWinApp * m_pTheApp ;

} ;

// The function to get the application instance.
inline JWinApp * JfxGetApp ( void )
{
    return ( JWinApp::m_pTheApp ) ;
}

// Some other helpful functions.
int JfxMessageBox ( UINT uiText , UINT nType = MB_OK ) ;
int JfxMessageBox ( LPCTSTR szText , UINT nType = MB_OK ) ;
int JfxMessageBox ( HWND hWnd , UINT uiText , UINT nType = MB_OK ) ;
int JfxMessageBox ( HWND hWnd , LPCTSTR szText , UINT nType = MB_OK ) ;


#endif  // _JWINAPP_H

