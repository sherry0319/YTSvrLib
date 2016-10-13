/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JCOMBOBOX_H
#define _JCOMBOBOX_H

class JComboBox : public JWnd
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JComboBox ( void )
    {
    }

    virtual ~JComboBox ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                           States and Checks
//////////////////////////////////////////////////////////////////////*/
public      :

    int AddString ( LPCTSTR szString )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_ADDSTRING     ,
                               0                ,
                               (LPARAM)szString  ) ) ;
    }

    int DeleteString ( int iIndex )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_DELETESTRING  ,
                               iIndex            ) ) ;
    }

    int FindString ( LPCTSTR szStr , int iStart = -1 )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_FINDSTRING    ,
                               (WPARAM)iStart   ,
                               (LPARAM)szStr     ) ) ;
    }

    int GetCount ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_GETCOUNT  ) ) ;
    }

    int GetCurSel ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_GETCURSEL ) ) ;
    }

    INT_PTR GetItemData ( int iIndex )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (INT_PTR)SendMessage ( CB_GETITEMDATA ,
                                        (WPARAM)iIndex  ) ) ;
    }

    int GetListBoxText ( int iIndex , LPCTSTR szText )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_GETLBTEXT     ,
                               (WPARAM)iIndex   ,
                               (LPARAM)szText    ) ) ;
    }

    int GetListBoxTextLength ( int iIndex )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_GETLBTEXTLEN ,
                               (WPARAM)iIndex   ) ) ;
    }

    int GetListBoxText ( int iIndex , JString & jStr )
    {
        int iRet = GetListBoxTextLength ( iIndex ) ;
        if ( iRet > 0 )
        {
            jStr.SetBufferLen ( iRet + 1 ) ;
            iRet = GetListBoxText ( iIndex , jStr.GetBuffer ( ) ) ;
        }
        return ( iRet ) ;
    }

    int InsertString ( int iIndex , LPCTSTR szText )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_INSERTSTRING  ,
                               (WPARAM)iIndex   ,
                               (LPARAM)szText    ) ) ;
    }

    void ResetContent ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        SendMessage ( CB_RESETCONTENT ) ;
    }

    int SelectString ( LPCTSTR szText , int iStart = -1 )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_SELECTSTRING ,
                               (WPARAM)iStart  ,
                               (LPARAM)szText   ) ) ;
    }

    int SetCurSel ( int iIndex )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_SETCURSEL , iIndex ) ) ;
    }

    int SetItemData ( int iIndex , void * pData )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( CB_SETITEMDATA ,
                               (WPARAM)iIndex ,
                               (LPARAM)pData   ) ) ;
    }

    int AddStringAndData ( LPCTSTR szString , void * pData )
    {
        int iIndex = AddString ( szString ) ;
        ASSERT ( CB_ERR != iIndex ) ;
#ifdef _DEBUG
        int iData =
#endif
        SetItemData ( iIndex ,  pData ) ;
        ASSERT ( CB_ERR != iData ) ;
        return ( iIndex ) ;
    }

    int InsertStringAndData ( int     iIndex   ,
                              LPCTSTR szString ,
                              void *  pData     )
    {
        int iRet = InsertString ( iIndex , szString ) ;
        ASSERT ( CB_ERR != iRet ) ;
        ASSERT ( CB_ERRSPACE != iRet ) ;
#ifdef _DEBUG
        int iIn =
#endif
        SetItemData ( iRet , pData ) ;
        ASSERT ( CB_ERR != iIn ) ;
        return ( iRet ) ;
    }

} ;

#endif  // _JCOMBOBOX_H


