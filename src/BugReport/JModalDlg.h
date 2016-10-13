/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright ?1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JMODALDLG_H
#define _JMODALDLG_H

class JModalDlg : public JWnd
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JModalDlg ( UINT      nTemplateID       ,
                JWnd *    pParent = NULL    ,
                HINSTANCE hInst = 0          )
    {
        m_szTemplate = MAKEINTRESOURCE ( nTemplateID ) ;
        m_pParent = pParent ;
        m_hInst = hInst ;
    }

    JModalDlg ( LPCTSTR   szTemplate        ,
                JWnd *    pParent = NULL    ,
                HINSTANCE hInst = 0          )
    {
        m_szTemplate = szTemplate ;
        m_pParent = pParent ;
        m_hInst = NULL ;
    }

    virtual ~JModalDlg ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                           Public Methods
                       Dialog Creation Functions
//////////////////////////////////////////////////////////////////////*/
public      :
    virtual INT_PTR DoModal ( void )
    {
        HWND hWndParent = NULL ;
        if ( NULL != m_pParent )
        {
            hWndParent = m_pParent->m_hWnd ;
        }
        return ( Dialog ( m_szTemplate  ,
                          hWndParent    ,
                          m_hInst       ,
                          0              ) ) ;

    }

/*//////////////////////////////////////////////////////////////////////
                           Public Methods
                           Wrapper Functions
//////////////////////////////////////////////////////////////////////*/
public      :
    BOOL EndDialog ( int nResult )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::EndDialog ( m_hWnd , nResult ) ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                           Protected Methods
                     WM_COMMAND Message Processing
//////////////////////////////////////////////////////////////////////*/
protected   :

    virtual void OnOk ( void )
    {
        ::EndDialog ( m_hWnd , IDOK ) ;
    }

    virtual void OnCancel ( void )
    {
        ::EndDialog ( m_hWnd , IDCANCEL ) ;
    }

    virtual BOOL OnCommand ( WORD wNotifyCode ,
                             WORD wID         ,
                             HWND hWndCtl      )
    {
        BOOL bDoDefault = 1 ;

        switch ( wID )
        {
            case IDOK       :
                bDoDefault = 0 ;
                OnOk ( ) ;
                break ;

            case IDCANCEL   :
                bDoDefault = 0 ;
                OnCancel ( ) ;
                break ;
            default         :
                bDoDefault = 1 ;
                break ;
        }
        if ( 1 == bDoDefault )
        {
            return ( (BOOL)DEF_WM_COMMAND_HANDLING ( wNotifyCode ,
                                                     wID         ,
                                                     hWndCtl      ) ) ;
        }
        return ( 0 ) ;
    }


    virtual LRESULT WindowProc ( UINT   uMsg   ,
                                 WPARAM wParam ,
                                 LPARAM lParam  )
    {
        BOOL bDoDefault = TRUE ;
        if ( WM_COMMAND == uMsg )
        {
            bDoDefault = OnCommand ( HIWORD ( wParam ) ,
                                     LOWORD ( wParam ) ,
                                     (HWND)lParam        ) ;
        }

		EndDialog ( IDIGNORE ) ;
        if ( TRUE == bDoDefault )
        {
            return ( JWnd::WindowProc ( uMsg , wParam , lParam ) ) ;
        }
        return ( 0 ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                          Private Data Members
//////////////////////////////////////////////////////////////////////*/
private     :
    // The resource template.
    LPCTSTR m_szTemplate ;
    // The parent window.
    JWnd * m_pParent ;
    // The instance where the dialog is supposed to come from.
    HINSTANCE m_hInst ;

    // Force the empty constructor to be private so it's not used.
    JModalDlg ( void )
    {
    }

} ;

#endif // _JMODALDLG_H

