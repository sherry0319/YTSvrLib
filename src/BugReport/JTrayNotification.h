/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JTRAYNOTIFICATION_H
#define _JTRAYNOTIFICATION_H

// Class to encapsulate all of the info needed to do a tray
// notification.

class JTrayNotification
{
public:

    JTrayNotification ( )
    {
        ZeroMemory ( &m_nid , sizeof ( m_nid ) ) ;
        m_nid.cbSize = sizeof ( m_nid ) ;
    }
    virtual ~JTrayNotification ( )
    {
        Delete ( ) ;
    }

    BOOL Add ( HWND   hwnd ,
               UINT   uID  ,
               UINT   uMsgCallback ,
               HICON  hicon ,
               PCTSTR pszTip = NULL)
    {
        m_nid.hWnd = hwnd;
        m_nid.uID = uID;
        m_nid.uFlags = NIF_ICON | NIF_MESSAGE |
                        (( pszTip == NULL) ? 0 : NIF_TIP ) ;
        m_nid.uCallbackMessage = uMsgCallback;
        m_nid.hIcon = hicon;
        if  (  pszTip != NULL )
        {
            lstrcpy ( m_nid.szTip , pszTip ) ;
        }
        return ( ::Shell_NotifyIcon ( NIM_ADD , &m_nid ) ) ;
    }

    BOOL Delete ( )
    {
        return ( ::Shell_NotifyIcon ( NIM_DELETE , &m_nid ) ) ;
    }

    HICON GetCurrentIcon ( )
    {
        return ( m_nid.hIcon ) ;
    }

    BOOL Modify ( PCTSTR pszTip)
    {
        m_nid.uFlags = NIF_TIP;
        lstrcpy ( m_nid.szTip , pszTip ) ;
        return ( ::Shell_NotifyIcon ( NIM_MODIFY , &m_nid ) ) ;
    }

    BOOL Modify ( HICON hicon , PCTSTR pszTip = NULL )
    {
        m_nid.uFlags = NIF_ICON |
                       ( ( pszTip == NULL) ? 0 : NIF_TIP ) ;
        m_nid.hIcon = hicon;
        if  (  pszTip != NULL )
        {
            lstrcpy ( m_nid.szTip , pszTip ) ;
        }
        return ( ::Shell_NotifyIcon ( NIM_MODIFY , &m_nid) ) ;
    }

    LRESULT ProcessTrayNotification ( WPARAM wParam , LPARAM lParam)
    {
        ASSERT ( wParam == m_nid.uID ) ;
        LRESULT lResult = 0;
        switch  (   ( UINT) lParam )
        {
            case WM_MOUSEMOVE :
                lResult = OnMouseMove (  ) ;
                break ;
            case WM_LBUTTONDOWN :
                lResult = OnLButtonDown (  ) ;
                break ;
            case WM_LBUTTONDBLCLK :
                lResult = OnLButtonDblClk (  ) ;
                break ;
            case WM_RBUTTONDOWN :
                lResult = OnRButtonDown (  ) ;
                break ;
            case WM_CONTEXTMENU :
                lResult = OnContextMenu (  ) ;
                break ;
        }
        return ( lResult ) ;
    }

protected:
    virtual LRESULT OnMouseMove ( )
    {
        return ( 0 ) ;
    }
    virtual LRESULT OnLButtonDown ( )
    {
        return ( 0 ) ;
    }
    virtual LRESULT OnLButtonDblClk ( )
    {
        return ( 0 ) ;
    }
    virtual LRESULT OnRButtonDown ( )
    {
        return ( 0 ) ;
    }
    virtual LRESULT OnRButtonDblClk ( )
    {
        return ( 0 ) ;
    }
    virtual LRESULT OnMButtonDown ( )
    {
        return ( 0 ) ;
    }
    virtual LRESULT OnMButtonDblClk ( )
    {
        return ( 0 ) ;
    }
    virtual LRESULT OnContextMenu ( )
    {
        return ( 0 ) ;
    }

public:
    NOTIFYICONDATA m_nid ;
};

#endif  // _JTRAYNOTIFICATION_H
