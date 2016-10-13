/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JMENU_H
#define _JMENU_H

class JMenu
{
/*//////////////////////////////////////////////////////////////////////
                      Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JMenu ( void )
    {
        m_hMenu = NULL ;
        m_bDestroy = FALSE ;
#ifdef _DEBUG
        m_bCheckMenuLeak = TRUE ;
#endif
    }

    virtual ~JMenu ( void )
    {
        if ( TRUE == m_bDestroy )
        {
            DestroyMenu ( ) ;
            m_hMenu = NULL ;
        }
#ifdef DEBUG
        if ( TRUE == m_bCheckMenuLeak )
        {
            ASSERT ( NULL == m_hMenu ) ;
        }
#endif
    }

/*//////////////////////////////////////////////////////////////////////
                        Public Operator Members
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HMENU()
    {
        return ( m_hMenu ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                       Menu Creation and Deletion
//////////////////////////////////////////////////////////////////////*/
public      :

    BOOL Attach ( HMENU hMenu , BOOL bDestroy = TRUE )
    {
        ASSERT ( NULL != hMenu ) ;
        m_hMenu = hMenu ;
        m_bDestroy = bDestroy ;
        return ( NULL != m_hMenu ) ;
    }

    HMENU Detach ( void )
    {
        HMENU hTemp = NULL ;
        if ( ( NULL != m_hMenu ) && ( TRUE == m_bDestroy ) )
        {
            DestroyMenu ( ) ;
            m_hMenu = NULL ;
            hTemp = NULL ;
        }
        else
        {
            hTemp = m_hMenu ;
            m_hMenu = NULL ;
        }
        return ( hTemp ) ;
    }

    BOOL CreateMenu ( void )
    {
        m_hMenu = ::CreateMenu ( ) ;
        ASSERT ( NULL != m_hMenu ) ;
        if ( NULL != m_hMenu )
        {
            m_bDestroy = TRUE ;
        }
        return ( NULL != m_hMenu ) ;
    }

    // This assumes you will be putting this popup on a
    // menu.  If you aren't set bDestroy to TRUE.
    BOOL CreatePopupMenu ( BOOL bDestroy = FALSE )
    {
        m_hMenu = ::CreatePopupMenu ( ) ;
        ASSERT ( NULL != m_hMenu ) ;
        m_bDestroy = bDestroy ;
#ifdef _DEBUG
        if ( FALSE == m_bDestroy )
        {
            m_bCheckMenuLeak = FALSE ;
        }
#endif
        return ( NULL != m_hMenu ) ;
    }

    BOOL DestroyMenu ( void )
    {
        ASSERT ( NULL != m_hMenu ) ;
        BOOL bRet = ::DestroyMenu ( m_hMenu ) ;
        ASSERT ( TRUE == bRet ) ;
        m_hMenu = NULL ;
        m_bDestroy = FALSE ;
        return ( bRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                      Menu Manipulation Functions
//////////////////////////////////////////////////////////////////////*/
public      :

    int GetMenuItemCount ( void )
    {
        ASSERT ( NULL != m_hMenu ) ;
        return ( ::GetMenuItemCount ( m_hMenu ) ) ;
    }

    BOOL AppendMenu ( UINT     uFlags         ,
                      UINT_PTR uIDNewItem = 0 ,
                      LPCTSTR  lpNewItem  = 0  )
    {
        ASSERT ( NULL != m_hMenu ) ;
        return ( ::AppendMenu ( m_hMenu     ,
                                uFlags      ,
                                uIDNewItem  ,
                                lpNewItem    ) ) ;
    }

    DWORD CheckMenuItem ( UINT uIDCheckItem , UINT uCheck )
    {
        ASSERT ( NULL != m_hMenu ) ;
        return ( ::CheckMenuItem ( m_hMenu , uIDCheckItem , uCheck ) ) ;
    }

    HMENU GetSubMenu ( int nPos )
    {
        ASSERT ( NULL != m_hMenu ) ;
        return ( ::GetSubMenu ( m_hMenu , nPos ) ) ;
    }

    // Optimized for tray menus.
    BOOL TrackPopupMenuFromMouse ( HWND hWnd                        ,
                                   UINT uiAnimation =
                                              TPM_HORPOSANIMATION |
                                              TPM_VERNEGANIMATION    )
    {
        POINT pt ;
        GetCursorPos ( &pt ) ;

        return ( TrackPopupMenu ( hWnd , pt.x , pt.y , uiAnimation ) ) ;
    }


    BOOL TrackPopupMenu ( HWND         hWnd                         ,
                          int          x                            ,
                          int          y                            ,
                          UINT         uiAnimationFlags =
                                              TPM_HORPOSANIMATION |
                                              TPM_VERNEGANIMATION   ,
                          UINT         uFlags = TPM_BOTTOMALIGN |
                                                //TPM_CENTERALIGN |
                                                TPM_LEFTBUTTON  |
                                                TPM_RIGHTBUTTON |
                                                TPM_RETURNCMD        )
    {
        ASSERT ( NULL != m_hMenu ) ;

        BOOL bIsFadeOn = FALSE ;

        if ( FALSE == ::SystemParametersInfo ( SPI_GETMENUFADE  ,
                                            0                ,
                                            &bIsFadeOn       ,
                                            FALSE             ) )
        {
            bIsFadeOn = FALSE ;
        }
        if ( TRUE == bIsFadeOn )
        {
            uFlags |= uiAnimationFlags ;
        }

        return ( ::TrackPopupMenu ( m_hMenu     ,
                                    uFlags      ,
                                    x           ,
                                    y           ,
                                    0           ,
                                    hWnd,
                                    NULL         ) ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                          Private Data Members
//////////////////////////////////////////////////////////////////////*/
private     :
    // The HMENU itself.
    HMENU m_hMenu ;
    // Destroy the menu in the destructor?
    BOOL m_bDestroy ;
#ifdef _DEBUG
    // Check for menu leaks in the destructor.
    BOOL m_bCheckMenuLeak ;
#endif
} ;


#endif  // _JMENU_H


