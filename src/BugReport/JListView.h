/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JLISTVIEW_H
#define _JLISTVIEW_H


class JListView : public JWnd
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JListView ( void )
    {
    }

    virtual ~JListView ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                        Public Creation Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    // You only need to set status bar styles in lSBStyles.
    HWND CreateListView ( LONG    lLVExtStyle ,
                          LONG    lStyle      ,
                          LPCTSTR lpszText    ,
                          JWnd *  pParent     ,
                          HMENU   hmID        ,
                          LPRECT  rRect        )
    {
        ASSERT ( NULL != pParent ) ;

        m_pParent = pParent ;

        // Create the status bar using Create so that the whole message
        // processing stuff gets hooked up.
        HWND hWndRet = CreateEx ( lLVExtStyle                 ,
                                  WC_LISTVIEW                 ,
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
                        Public List View Styles
//////////////////////////////////////////////////////////////////////*/
public      :
    void SetReportView ( void )
    {
        SetSimpleStyle ( LVS_TYPEMASK , LVS_REPORT ) ;
    }

    void SetIconView ( void )
    {
        SetSimpleStyle ( LVS_TYPEMASK , LVS_ICON ) ;
    }

    void SetSmallIconView ( void )
    {
        SetSimpleStyle ( LVS_TYPEMASK , LVS_SMALLICON ) ;
    }

    void SetListView ( void )
    {
        SetSimpleStyle ( LVS_TYPEMASK , LVS_LIST ) ;
    }

    void SetAlignLeft ( void )
    {
        SetSimpleStyle ( LVS_ALIGNMASK , LVS_ALIGNLEFT ) ;
    }

    void SetAlignTop ( void )
    {
        SetSimpleStyle ( LVS_ALIGNMASK , LVS_ALIGNTOP  ) ;
    }

    void SetAlwaysShowSelection ( void )
    {
        SetSimpleStyle ( 0 , LVS_SHOWSELALWAYS ) ;
    }

    void SetSingleSelect ( void )
    {
        SetSimpleStyle ( 0 , LVS_SINGLESEL ) ;
    }

    void SetFullRowSelect ( void )
    {
        SetExtendedStyle ( LVS_EX_FULLROWSELECT ) ;
    }

    // Sets the particular style for the list view.
    void SetSimpleStyle ( DWORD dwMask , DWORD dwStyle )
    {
        LONG_PTR dwCurrStyle = GetWindowLongPtr ( GWL_STYLE ) ;
        if ( 0 != dwMask )
        {
            SetWindowLongPtr ( GWL_STYLE                            ,
                               ( dwCurrStyle & ~dwMask ) | dwStyle   ) ;
        }
        else
        {
            SetWindowLongPtr ( GWL_STYLE , dwCurrStyle | dwStyle ) ;
        }
    }

    // Sets an extended style.
    LRESULT SetExtendedStyle ( DWORD dwExStyle , DWORD dwExMask = 0 )
    {
        return ( SendMessage ( LVM_SETEXTENDEDLISTVIEWSTYLE ,
                               dwExMask                     ,
                               dwExStyle                     ) ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Column Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    int InsertColumn ( int iCol , LPLVCOLUMN pCol )
    {
        return ( (int)SendMessage ( LVM_INSERTCOLUMN ,
                               iCol             ,
                               (LPARAM)pCol      ) ) ;
    }

    BOOL SetColumnWidth ( int iCol , int iWidth )
    {
        return ( (BOOL)SendMessage ( LVM_SETCOLUMNWIDTH       ,
                               iCol                     ,
                               MAKELPARAM ( iWidth , 0 ) ) ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Item Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    int GetSelectionMark ( void )
    {
        return ( ListView_GetSelectionMark ( m_hWnd ) ) ;
    }

    int GetNextItem ( int iStart , UINT flags )
    {
        return ( ListView_GetNextItem ( m_hWnd , iStart , flags ) ) ;
    }

    int SetSelectionMark ( int iIndex )
    {
        return ( ListView_SetSelectionMark ( m_hWnd , iIndex ) ) ;
    }

    int GetTopIndex ( void )
    {
        return ( ListView_GetTopIndex ( m_hWnd ) ) ;
    }

    BOOL EnsureVisible ( int iIndex , BOOL fPartialOK = TRUE )
    {
        return ( ListView_EnsureVisible ( m_hWnd     ,
                                          iIndex     ,
                                          fPartialOK  ) ) ;
    }

    BOOL DeleteAllItems ( void )
    {
        return ( ListView_DeleteAllItems ( m_hWnd ) ) ;
    }

    BOOL DeleteItem ( int iItem )
    {
        return ( ListView_DeleteItem ( m_hWnd , iItem ) ) ;
    }

    BOOL SetItem ( LPLVITEM pItem )
    {
        return ( ListView_SetItem ( m_hWnd , pItem ) ) ;
    }

    BOOL GetItem ( LPLVITEM pItem )
    {
        return ( ListView_GetItem ( m_hWnd , pItem ) ) ;
    }

    void SetItemState ( int i , UINT state , UINT mask )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        ListView_SetItemState ( m_hWnd , i , state , mask ) ;
    }

    int InsertItem ( LPLVITEM pItem )
    {
        return ( ListView_InsertItem ( m_hWnd , pItem ) ) ;
    }

    int GetItemCount ( void )
    {
        return ( ListView_GetItemCount ( m_hWnd ) ) ;
    }


/*//////////////////////////////////////////////////////////////////////
                        Imagelist Public Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    HIMAGELIST SetImageList ( HIMAGELIST hIml , int iImageList )
    {
        return ( ListView_SetImageList ( m_hWnd , hIml , iImageList ) );
    }



/*//////////////////////////////////////////////////////////////////////
                        Private Data Methods
//////////////////////////////////////////////////////////////////////*/
private     :
    // The parent window.
    JWnd * m_pParent ;

} ;

#endif  // _JLISTVIEW_H

