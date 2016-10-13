/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JSTATUSBAR_H
#define _JSTATUSBAR_H


class JStatusBar : public JWnd
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JStatusBar ( void )
    {
        m_iMinHeight = 20 ;
        m_pParent = NULL ;
    }

    virtual ~JStatusBar ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                        Public Creation Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    // You only need to set status bar styles in lSBStyles.
    HWND CreateStatusWindow ( LONG    lSBStyle  ,
                              LPCTSTR lpszText  ,
                              JWnd *  pParent   ,
                              HMENU   hmID       )
    {
        ASSERT ( NULL != pParent ) ;

        RECT stRect ;
        pParent->GetClientRect ( stRect ) ;
        stRect.top = stRect.bottom - m_iMinHeight ;

        m_pParent = pParent ;

        // Create the status bar using CreateEx so that the whole
        // message processing stuff gets hooked up.
        HWND hWndRet = CreateEx ( lSBStyle                         ,
                                  STATUSCLASSNAME                  ,
                                  lpszText                         ,
                                  WS_CHILD |
                                      WS_VISIBLE |
                                      WS_CLIPSIBLINGS              ,
                                  stRect.left                      ,
                                  stRect.top                       ,
                                  stRect.right - stRect.left       ,
                                  stRect.bottom - stRect.top       ,
                                  m_pParent                        ,
                                  hmID                              ) ;
        ASSERT ( NULL != hWndRet ) ;
        if ( NULL != hWndRet )
        {
            SetMinHeight ( m_iMinHeight ) ;
        }
        return ( hWndRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                        Public Manipulation Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    void SetMinHeight ( int iMinHeight )
    {
        SendMessage ( SB_SETMINHEIGHT , iMinHeight , 0 ) ;
        m_iMinHeight = iMinHeight ;
        SendMessage ( WM_SIZE , 0 , 0 ) ;
    }

    int GetTotalHeight ( void )
    {
        RECT stRect ;
        GetWindowRect ( stRect ) ;
        return ( stRect.bottom - stRect.top ) ;
    }

    void MoveWindowInParent ( void )
    {
        ASSERT ( NULL != m_pParent ) ;

        RECT stRect ;
        m_pParent->GetClientRect ( stRect ) ;
        stRect.top = stRect.bottom - GetTotalHeight ( )  ;

        SetWindowPos ( NULL                         ,
                       stRect.left                  ,
                       stRect.top                   ,
                       stRect.right - stRect.left   ,
                       stRect.bottom - stRect.top   ,
                       SWP_NOZORDER                  ) ;
        InvalidateRect ( NULL , TRUE ) ;
    }

    void SetSimpleMode ( BOOL bIsSimple )
    {
        SendMessage ( SB_SIMPLE , bIsSimple , 0 ) ;
    }

    void SetText ( LPCTSTR szText , int iPane = 0 , int iStyle = 0 )
    {
        SendMessage ( SB_SETTEXT , iPane | iStyle , (LPARAM)szText ) ;
    }

    void SetDefaultText ( LPCTSTR szText )
    {
        m_strDefault = szText ;
    }

    void SetDefaultText ( UINT uiResString )
    {
        m_strDefault.LoadString ( uiResString ) ;
    }

    // Call this function from your WM_MENUSELECT processing to get
    // the menu help strings shown in the status bar.  This function
    // assumes that all status bar helper text has the same ID as the
    // menu ID.
    virtual LRESULT OnMenuSelect ( UINT  nItemID  ,
                                   UINT  nFlags   ,
                                   HMENU hSysMenu  ) ;

/*//////////////////////////////////////////////////////////////////////
                        Private Data Methods
//////////////////////////////////////////////////////////////////////*/
private     :
    // The minimum height.
    int m_iMinHeight ;
    // The parent window.
    JWnd * m_pParent ;

    // The default text for the first pane.
    JString m_strDefault ;


} ;

#endif  // _JSTATUSBAR_H

