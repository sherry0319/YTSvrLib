/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JLISTBOX_H
#define _JLISTBOX_H

class JListBox : public JWnd
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JListBox ( void )
    {
    }

    virtual ~JListBox ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    int GetCount ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_GETCOUNT ) ) ;
    }

    int GetCurSel ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_GETCURSEL ) ) ;
    }

    int SetCurSel ( int iIndex )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_SETCURSEL , iIndex ) ) ;
    }

    int GetText ( int iIndex , LPTSTR szText )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_GETTEXT       ,
                                    iIndex           ,
                                    (LPARAM)szText )  );
    }

    int GetTextLen ( int iIndex )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_GETTEXTLEN ) ) ;
    }

    int AddString ( LPCTSTR szText )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_ADDSTRING    ,
                                    0               ,
                                    (LPARAM)szText ) ) ;
    }

    int DeleteString ( int iIndex )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_DELETESTRING , iIndex ) ) ;
    }

    int InsertString ( int iIndex , LPTSTR szText )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_INSERTSTRING ,
                                    iIndex          ,
                                    (LPARAM)szText   ) );
    }

    int SetItemData ( int iIndex , DWORD dwData )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (int)SendMessage ( LB_SETITEMDATA ,
                                    iIndex         ,
                                    (LPARAM)dwData  ) );
    }

    int AddStringAndData ( LPCTSTR szText , DWORD dwData )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        int iIndex = AddString ( szText ) ;
        if ( LB_ERR != iIndex )
        {
            if ( LB_ERR == SetItemData ( iIndex , dwData ) )
            {
                DeleteString ( iIndex ) ;
                return ( LB_ERR ) ;
            }
        }
        return ( iIndex ) ;
    }

    void ResetContent ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        SendMessage ( LB_RESETCONTENT ) ;
    }


} ;

#endif  // _JLISTBOX_H



