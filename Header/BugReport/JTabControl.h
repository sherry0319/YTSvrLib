/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JTABCONTROL_H
#define _JTABCONTROL_H


class JTabControl : public JWnd
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JTabControl ( void )
    {
    }

    virtual ~JTabControl ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                        Public Creation Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    HWND CreateTabControl ( LONG    lTCExtStyle ,
                            LONG    lStyle      ,
                            LPCTSTR lpszText    ,
                            JWnd *  pParent     ,
                            HMENU   hmID        ,
                            LPRECT  rRect        )
    {
        ASSERT ( NULL != pParent ) ;
        if ( NULL == pParent )
        {
            return ( NULL ) ;
        }

        m_pParent = pParent ;

        // Create the status bar using Create so that the whole message
        // processing stuff gets hooked up.
        HWND hWndRet = CreateEx ( lTCExtStyle                 ,
                                  WC_TABCONTROL               ,
                                  lpszText                    ,
                                  WS_CHILD |
                                      WS_VISIBLE |
                                      WS_CLIPSIBLINGS |
                                      lStyle                  ,
                                  rRect->left                 ,
                                  rRect->top                  ,
                                  rRect->right - rRect->left  ,
                                  rRect->bottom - rRect->top  ,
                                  m_pParent                   ,
                                  (HMENU)hmID                  ) ;
        ASSERT ( NULL != hWndRet ) ;
        return ( hWndRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                   Public Tab Control Extended Styles
//////////////////////////////////////////////////////////////////////*/
public      :
    // Sets an extended style.
    LRESULT SetExtendedStyle ( DWORD dwExStyle , DWORD dwExMask = 0 )
    {
        return ( SendMessage ( TCM_SETEXTENDEDSTYLE ,
                               dwExMask             ,
                               dwExStyle             ) ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                           Public Tab Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    BOOL DeleteAllItems ( void )
    {
        return ( TabCtrl_DeleteAllItems ( m_hWnd ) ) ;
    }

    BOOL DeleteItem ( int iItem )
    {
        return ( TabCtrl_DeleteItem ( m_hWnd , iItem ) ) ;
    }

    BOOL SetItem ( int iItem , LPTCITEM pItem )
    {
        return ( TabCtrl_SetItem ( m_hWnd , iItem , pItem ) ) ;
    }

    BOOL GetItem ( int iItem , LPTCITEM pItem )
    {
        return ( TabCtrl_GetItem ( m_hWnd , iItem , pItem ) ) ;
    }

    int InsertItem ( int iItem , LPTCITEM pItem )
    {
        return ( TabCtrl_InsertItem ( m_hWnd , iItem , pItem ) ) ;
    }

    int GetItemCount ( void )
    {
        return ( TabCtrl_GetItemCount ( m_hWnd ) ) ;
    }

    int GetCurSel ( void )
    {
        return ( TabCtrl_GetCurSel ( m_hWnd ) ) ;
    }

    int SetCurSel ( int iItem )
    {
        return ( TabCtrl_SetCurSel ( m_hWnd , iItem ) ) ;
    }

    BOOL GetItemRect ( int iItem , RECT * pRect )
    {
        return ( TabCtrl_GetItemRect ( m_hWnd , iItem , pRect ) ) ;
    }

    BOOL SetItemSize ( int cx , int cy )
    {
        return ( TabCtrl_SetItemSize ( m_hWnd , cx , cy ) ) ;
    }

    int GetCurFocus ( void )
    {
        return ( TabCtrl_GetCurFocus ( m_hWnd ) ) ;
    }

    void SetCurFocus ( int iItem )
    {
        TabCtrl_SetCurFocus ( m_hWnd , iItem ) ;
    }

    int HitTest ( LPTCHITTESTINFO pInfo )
    {
        return ( TabCtrl_HitTest ( m_hWnd , pInfo ) ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                        Imagelist Public Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    HIMAGELIST SetImageList ( HIMAGELIST hIml )
    {
        return ( TabCtrl_SetImageList ( m_hWnd , hIml ) );
    }

    void RemoveImage ( int iImage )
    {
        TabCtrl_RemoveImage ( m_hWnd , iImage ) ;
    }



/*//////////////////////////////////////////////////////////////////////
                        Private Data Methods
//////////////////////////////////////////////////////////////////////*/
private     :
    // The parent window.
    JWnd * m_pParent ;

} ;

#endif  // _JTABCONTROL_H

