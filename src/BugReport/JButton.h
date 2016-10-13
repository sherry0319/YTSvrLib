/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JBUTTON_H
#define _JBUTTON_H

class JButton : public JWnd
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JButton ( void )
    {
    }

    virtual ~JButton ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                           States and Checks
//////////////////////////////////////////////////////////////////////*/
public      :

    UINT GetState ( ) const
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (UINT)::SendMessage ( m_hWnd , BM_GETSTATE  , 0 , 0 ) ) ;
    }

    void SetState ( BOOL bHighlight )
    {
        ASSERT (  ::IsWindow ( m_hWnd ) ) ;
        ::SendMessage ( m_hWnd , BM_SETSTATE , bHighlight , 0 ) ;
    }

    int GetCheck ( ) const
    {
        ASSERT (  ::IsWindow ( m_hWnd ) ) ;
        return ( (int)::SendMessage ( m_hWnd , BM_GETCHECK , 0 , 0 ) ) ;
    }

    void SetCheck(int nCheck)
    {
        ::SendMessage ( m_hWnd , BM_SETCHECK , nCheck , 0 ) ;
    }

    UINT GetButtonStyle ( ) const
    {
        ASSERT (  ::IsWindow ( m_hWnd ) ) ;
        return ( (UINT)::GetWindowLong ( m_hWnd , GWL_STYLE ) & 0xFF ) ;
    }

    void SetButtonStyle ( UINT nStyle , BOOL bRedraw )
    {
        ASSERT (  ::IsWindow ( m_hWnd ) ) ;
        ::SendMessage ( m_hWnd          ,
                        BM_SETSTYLE     ,
                        nStyle          ,
                        (LPARAM)bRedraw  ) ;
    }

    HICON SetIcon(HICON hIcon)
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (HICON)::SendMessage ( m_hWnd          ,
                                        BM_SETIMAGE     ,
                                        IMAGE_ICON      ,
                                        (LPARAM)hIcon    ) ) ;
    }

    HICON GetIcon ( ) const
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (HICON)::SendMessage ( m_hWnd      ,
                                        BM_GETIMAGE ,
                                        IMAGE_ICON  ,
                                        0L           ) ) ;
    }

    HBITMAP SetBitmap(HBITMAP hBitmap)
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (HBITMAP)::SendMessage ( m_hWnd            ,
                                          BM_SETIMAGE       ,
                                          IMAGE_BITMAP      ,
                                          (LPARAM)hBitmap    ) ) ;
    }

    HBITMAP GetBitmap ( ) const
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (HBITMAP)::SendMessage ( m_hWnd        ,
                                          BM_GETIMAGE   ,
                                          IMAGE_BITMAP  ,
                                          0L             ) ) ;
    }

    HCURSOR SetCursor(HCURSOR hCursor)
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (HCURSOR)::SendMessage ( m_hWnd        ,
                                          BM_SETIMAGE   ,
                                          IMAGE_CURSOR  ,
                                          (LPARAM)hCursor ) ) ;
    }

    HCURSOR GetCursor ( )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (HCURSOR)::SendMessage ( m_hWnd        ,
                                          BM_GETIMAGE   ,
                                          IMAGE_CURSOR  ,
                                          0L             ) ) ;
    }

    BOOL IsChecked ( void )
    {
        return ( BST_CHECKED == GetCheck ( ) ) ;
    }

    void SetChecked ( void )
    {
        SetCheck ( BST_CHECKED ) ;
    }

} ;

#endif  // _JBUTTON_H


