/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JWND_H
#define _JWND_H

#ifdef MULTITHREADJWND
#define STATICTLS __declspec(thread)
#else
#define STATICTLS
#endif

/*//////////////////////////////////////////////////////////////////////
                             Global Defines
//////////////////////////////////////////////////////////////////////*/
// The default window class registered by this library.
#define JWNDDEFCLASS _T ( "JWndClass" )
class JPoint : public tagPOINT
{
public      :
    JPoint ( )
    {
        x = 0 ;
        y = 0 ;
    }
} ;

class JRect : public tagRECT
{
public      :
    JRect ( )
    {
        left = 0 ;
        right = 0 ;
        top = 0 ;
        bottom = 0 ;
    }

    JRect ( int iLeft , int iTop , int iRight , int iBottom )
    {
        left = iLeft ;
        right = iRight ;
        top = iTop ;
        bottom = iBottom ;
    }

    JRect ( LPCRECT lpRC )
    {
        left = lpRC->left ;
        right = lpRC->right ;
        top = lpRC->top ;
        bottom = lpRC->bottom ;
    }

    JRect ( RECT & Rect )
    {
        left = Rect.left ;
        right = Rect.right ;
        top = Rect.top ;
        bottom = Rect.bottom ;
    }

    int Width ( void ) const
    {
        return ( right - left ) ;
    }

    int Height ( void ) const
    {
        return ( bottom - top ) ;
    }

    JRect & operator = ( const JRect & t )
    {
        left = t.left ;
        right = t.right ;
        top = t.top ;
        bottom = t.bottom ;
        return ( *this ) ;
    }

    BOOL PtInRect ( POINT pt )
    {
        return ( ::PtInRect ( this , pt ) ) ;
    }
} ;


// My simple window class.
class JWnd
{

/*//////////////////////////////////////////////////////////////////////
                      Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JWnd ( void )
    {
        m_hWnd = NULL ;
        m_pOrigWndProc = NULL ;

        m_uMsg = 0 ;
        m_wParam = 0 ;
        m_lParam = 0 ;

        m_bDefWndProcCalled = FALSE ;
    }

    virtual ~JWnd ( void )
    {
        if ( ( NULL != m_pOrigWndProc ) &&
             ( ::IsWindow ( m_hWnd )  )    )
        {
            Detach ( ) ;
        }
    }

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                      Creating Windows and Dialogs
NOTE NOTE NOTE: If the hInst value is zero, the default resource module
                is used.
//////////////////////////////////////////////////////////////////////*/
public      :
    // Create a modal dialog from an integer resource ID.
    INT_PTR Dialog ( int       nTemplateID       ,
                     HWND      hWndParent = NULL ,
                     HINSTANCE hInst = 0         ,
                     LPARAM    lParam = 0         )
    {
        // Just call on through.
        return ( Dialog ( MAKEINTRESOURCE ( nTemplateID ) ,
                          hWndParent                      ,
                          hInst                           ,
                          lParam                           ) ) ;
    }

    // Create a modal dialog from a string resource ID.
    INT_PTR Dialog ( LPCTSTR   szTemplate        ,
                     HWND      hWndParent = NULL ,
                     HINSTANCE hInst = 0         ,
                     LPARAM    lParam = 0         ) ;

    // Create a modeless dialog from an integer resource ID.
    HWND ModelessDialog ( int       nTemplateID       ,
                          HWND      hWndParent = NULL ,
                          HINSTANCE hInst = 0         ,
                          LPARAM lParam = 0            )
    {
        // Just call on through.
        return ( ModelessDialog ( MAKEINTRESOURCE ( nTemplateID ) ,
                                  hWndParent                      ,
                                  hInst                           ,
                                  lParam                           ) ) ;
    }

    // Create a modaless dialog from a string resource ID.
    HWND ModelessDialog ( LPCTSTR   szTemplate        ,
                          HWND      hWndParent = NULL ,
                          HINSTANCE hInst = 0         ,
                          LPARAM    lParam = 0         ) ;

    // Attaches a JWnd object to an existing HWND.
    BOOL Attach ( HWND hWnd )
    {
        ASSERT ( ::IsWindow ( hWnd ) ) ;
        if ( FALSE == ::IsWindow ( hWnd ) )
        {
            return ( FALSE ) ;
        }
        // Make sure I'm not about to double attach....
        if ( ::GetWindowLongPtr ( hWnd , GWLP_WNDPROC ) ==
                                               (LONG_PTR)CommonWndProc )
        {
            ASSERT ( !"Double attach to an HWND!!" ) ;
            return ( FALSE ) ;
        }
        m_hWnd = hWnd ;

// Gotta love it.  I've NEVER found a way to cast away the /Wp64
// warnings with the functions Microsoft tells you to use for
// Win64 readiness!
#pragma warning ( disable : 4244 )
        m_pOrigWndProc = ::SetWindowLongPtr( m_hWnd                  ,
                                             GWLP_WNDPROC            ,
                                             (LONG_PTR)CommonWndProc );
        ::SetWindowLongPtr ( m_hWnd , GWLP_USERDATA , (LONG_PTR)this );
#pragma warning ( default: 4244 )

        return ( TRUE ) ;
    }

    // Attach to a dialog control of the specified window.
    BOOL Attach ( HWND hWnd , int nIDDlgItem )
    {
        ASSERT ( ::IsWindow ( hWnd ) ) ;
        HWND hWndChild = ::GetDlgItem ( hWnd , nIDDlgItem ) ;
        if ( NULL == hWndChild )
        {
            return ( FALSE ) ;
        }
        return ( Attach ( hWndChild ) ) ;
    }

    // Detaches from a previously attached window.
    void Detach ( )
    {
        if ( NULL != m_pOrigWndProc )
        {

// Gotta love it.  I've NEVER found a way to cast away the /Wp64
// warnings with the functions Microsoft tells you to use for
// Win64 readiness!
#pragma warning ( disable : 4244 )
            ::SetWindowLongPtr ( m_hWnd                   ,
                                 GWLP_WNDPROC             ,
                                 (LONG_PTR)m_pOrigWndProc  ) ;
            ::SetWindowLongPtr ( m_hWnd         ,
                                 GWLP_USERDATA  ,
                                 NULL            ) ;
#pragma warning ( default: 4244 )
        }
    }

    // For regular windows, allows derived classes to influence the
    // window creation.
    virtual BOOL PreCreateWindow ( CREATESTRUCT & cs ) ;
    BOOL CreateFromTemplate ( PCTSTR    pszTemplate        ,
                              HWND      hwndParent = NULL  ,
                              HINSTANCE hInst = 0           ) ;

    BOOL CreateFromTemplate ( int       nTemplateID       ,
                              HWND      hWndParent = NULL ,
                              HINSTANCE hInst = 0          )
    {
        return ( CreateFromTemplate ( MAKEINTRESOURCE ( nTemplateID ) ,
                                      hWndParent                      ,
                                      hInst                          ));
    }

    // The real, hard-core create!
    HWND CreateEx ( DWORD     dwExStyle       ,
                    LPCTSTR   szClassName     ,
                    LPCTSTR   szWindowName    ,
                    DWORD     dwStyle         ,
                    int       x               ,
                    int       y               ,
                    int       nWidth          ,
                    int       nHeight         ,
                    JWnd *    pParent         ,
                    HMENU     nIDorHMenu      ,
                    HINSTANCE hInstance = 0       ,
                    LPVOID    lpParam = NULL   ) ;

    // The Create for simple top level windows.
    HWND Create ( LPCTSTR szName , DWORD dwStyle ) ;

/*//////////////////////////////////////////////////////////////////////
                        Public Operator Members
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HWND()
    {
        return ( m_hWnd ) ;
    }


/*//////////////////////////////////////////////////////////////////////
                          Public Data Members
//////////////////////////////////////////////////////////////////////*/
public      :
    // The usual HWND.
    HWND m_hWnd ;

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                            Helper Functions
//////////////////////////////////////////////////////////////////////*/
public      :
    // Centers the window.  If NULL, centers on the primary desktop.
    BOOL CenterWindow ( HWND hWndCenter = NULL   ,
                        BOOL CurrMonitor = FALSE  ) ;

/*//////////////////////////////////////////////////////////////////////
                             Public Methods
                         HWND Wrapper Functions
//////////////////////////////////////////////////////////////////////*/
public      :
    BOOL SetWindowText ( LPCTSTR szText )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::SetWindowText ( m_hWnd , szText ) ) ;
    }

    int GetWindowText ( LPTSTR lpString , int nMaxCount )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetWindowText ( m_hWnd , lpString , nMaxCount ) ) ;
    }

    int GetWindowText ( JString & cString )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        int iLen = GetWindowTextLength ( ) ;
        if ( iLen > 0 )
        {
            cString.SetBufferLen ( iLen + 1 ) ;
            GetWindowText ( cString.GetBuffer ( ) , iLen ) ;
        }
        return ( iLen ) ;
    }

    int GetWindowTextLength ( )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetWindowTextLength ( m_hWnd ) ) ;
    }

    LONG_PTR GetWindowLongPtr ( int nIndex )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetWindowLongPtr ( m_hWnd , nIndex ) ) ;
    }

    LONG_PTR SetWindowLongPtr ( int nIndex , LONG_PTR dwNewLong )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
// Gotta love it.  I've NEVER found a way to cast away the /Wp64
// warnings with the functions Microsoft tells you to use for
// Win64 readiness!
#pragma warning ( disable : 4244 )
        return ( ::SetWindowLongPtr ( m_hWnd                ,
                                      nIndex                ,
                                      (LONG_PTR)dwNewLong )  ) ;
#pragma warning ( default: 4244 )
    }

    BOOL GetWindowRect ( LPRECT lpRect )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetWindowRect ( m_hWnd , lpRect ) ) ;
    }

    BOOL GetWindowRect ( RECT & rRect )
    {
        return ( GetWindowRect ( &rRect ) ) ;
    }

    BOOL GetClientRect ( LPRECT lpRect )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetClientRect ( m_hWnd , lpRect ) ) ;
    }

    BOOL GetClientRect ( RECT & rRect )
    {
        return ( GetClientRect ( &rRect ) ) ;
    }

    // Gets the window rect for a child window.
    BOOL GetItemWindowRect ( int nIDDlgItem , LPRECT lpRect )
    {
        HWND hWndChild = GetDlgItem ( nIDDlgItem ) ;
        if ( NULL == hWndChild )
        {
            return ( FALSE ) ;
        }
        return ( ::GetWindowRect ( hWndChild , lpRect ) ) ;
    }

    BOOL GetItemWindowRect ( int nIDDlgItem , RECT & rRect )
    {
        return ( GetItemWindowRect ( nIDDlgItem , &rRect ) ) ;
    }

    HWND GetParent ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetParent ( m_hWnd ) ) ;
    }

    BOOL DestroyWindow ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::DestroyWindow ( m_hWnd ) ) ;
    }

    LRESULT SendMessage ( UINT   uMsg       ,
                          WPARAM wParam = 0 ,
                          LPARAM lParam = 0  )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::SendMessage ( m_hWnd , uMsg , wParam , lParam ) ) ;
    }

    BOOL PostMessage ( UINT   uMsg       ,
                       WPARAM wParam = 0 ,
                       LPARAM lParam = 0  )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::PostMessage ( m_hWnd , uMsg , wParam , lParam ) ) ;
    }

    HICON SetIcon ( HICON hIcon , BOOL bBigIcon )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( (HICON)::SendMessage ( m_hWnd                  ,
                                        WM_SETICON              ,
                                        bBigIcon ? ICON_BIG
                                                 : ICON_SMALL   ,
                                        (LPARAM)hIcon            ) ) ;
    }

    void SetFont ( HFONT hFont , BOOL bRedraw )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        ::SendMessage ( m_hWnd , WM_SETFONT , (WPARAM)hFont , bRedraw );
    }

    HWND GetDlgItem ( int nIDDlgItem )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetDlgItem ( m_hWnd , nIDDlgItem ) ) ;
    }

    UINT GetDlgItemInt ( int    nIDDlgItem          ,
                         BOOL * lpTranslated = NULL ,
                         BOOL   bSigned      = TRUE  )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        BOOL bLocal ;
        UINT uiRet = ::GetDlgItemInt ( m_hWnd     ,
                                       nIDDlgItem ,
                                       &bLocal    ,
                                       bSigned     ) ;
        if ( NULL != lpTranslated )
        {
            *lpTranslated = bLocal ;
        }
        return ( uiRet ) ;
    }

    BOOL ShowWindow ( int nCmdShow )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::ShowWindow ( m_hWnd , nCmdShow ) ) ;
    }

    BOOL UpdateWindow ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::UpdateWindow ( m_hWnd ) ) ;
    }

    BOOL IsWindow ( void )
    {
        return ( ::IsWindow ( m_hWnd ) ) ;
    }

    BOOL EnableWindow ( BOOL bEnabled = TRUE )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::EnableWindow ( m_hWnd , bEnabled ) ) ;
    }

    BOOL IsWindowEnabled ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::IsWindowEnabled ( m_hWnd ) ) ;
    }

    HWND SetFocus ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::SetFocus ( m_hWnd ) ) ;
    }

    BOOL SetForegroundWindow ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::SetForegroundWindow ( m_hWnd ) ) ;
    }

    BOOL MoveWindow ( int  X                ,
                      int  Y                ,
                      int  nWidth           ,
                      int  nHeight          ,
                      BOOL bRepaint = TRUE   )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::MoveWindow ( m_hWnd ,
                                X         ,
                                Y         ,
                                nWidth    ,
                                nHeight   ,
                                bRepaint   ) ) ;
    }

    BOOL MoveWindow ( const RECT & rect, BOOL bRepaint = TRUE )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( MoveWindow ( rect.left              ,
                              rect.top               ,
                              rect.right - rect.left ,
                              rect.bottom - rect.top ,
                              bRepaint                ) ) ;
    }

    HDC BeginPaint ( LPPAINTSTRUCT lpPaint )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::BeginPaint ( m_hWnd , lpPaint ) ) ;
    }

    BOOL EndPaint ( LPPAINTSTRUCT lpPaint )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::EndPaint ( m_hWnd , lpPaint ) ) ;
    }
    
    HDC GetDC ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetDC ( m_hWnd ) ) ;
    }
    
    BOOL ReleaseDC ( HDC hDC )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::ReleaseDC ( m_hWnd , hDC ) ) ;
    }

    BOOL InvalidateRect ( CONST RECT * lpRect = NULL ,
                          BOOL bErase = FALSE          )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::InvalidateRect ( m_hWnd , lpRect , bErase ) ) ;
    }

    HMENU GetSystemMenu ( BOOL bRevert = FALSE )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetSystemMenu ( m_hWnd , bRevert ) ) ;
    }

    BOOL DrawMenuBar ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::DrawMenuBar ( m_hWnd ) ) ;
    }

    BOOL GetWindowPlacement ( WINDOWPLACEMENT * lpwndpl )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::GetWindowPlacement ( m_hWnd , lpwndpl ) ) ;
    }

    BOOL SetWindowPlacement ( WINDOWPLACEMENT * lpwndpl )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::SetWindowPlacement ( m_hWnd , lpwndpl ) ) ;
    }

    BOOL SetWindowPos ( HWND hWndInsertAfter ,
                        int  X               ,
                        int  Y               ,
                        int  cx              ,
                        int  cy              ,
                        UINT uFlags            )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::SetWindowPos ( m_hWnd          ,
                                  hWndInsertAfter ,
                                  X               ,
                                  Y               ,
                                  cx              ,
                                  cy              ,
                                  uFlags           ) ) ;
    }

    UINT_PTR SetTimer ( UINT      nIDEvent           ,
                    UINT      uElapse            ,
                    TIMERPROC lpTimerFunc = NULL  )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::SetTimer ( m_hWnd      ,
                              nIDEvent    ,
                              uElapse     ,
                              lpTimerFunc  ) ) ;
    }

    BOOL KillTimer ( UINT uiIDEvent )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::KillTimer ( m_hWnd , uiIDEvent ) ) ;
    }

    BOOL IsWindowVisible ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::IsWindowVisible ( m_hWnd ) ) ;
    }

    BOOL IsIconic ( void )
    {
        ASSERT ( ::IsWindow ( m_hWnd ) ) ;
        return ( ::IsIconic ( m_hWnd ) ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                           Protected Methods
                       Default Message Processing
//////////////////////////////////////////////////////////////////////*/
protected   :
    // Simple default window procedure.
    LRESULT DefWindowProc ( void ) ;
    // Window procedure with parameters.
    LRESULT DefWindowProc ( UINT uMsg , WPARAM wParam , LPARAM lParam );
    // The window procedure for the class.
    virtual LRESULT WindowProc ( UINT   uMsg   ,
                                 WPARAM wParam ,
                                 LPARAM lParam  ) ;

/*//////////////////////////////////////////////////////////////////////
                         Protected Data Members
//////////////////////////////////////////////////////////////////////*/
protected   :
    // The current windows message being processed along with it's
    // parameters.
    UINT   m_uMsg ;
    WPARAM m_wParam ;
    LPARAM m_lParam ;

/*//////////////////////////////////////////////////////////////////////
                          Private Member Data
//////////////////////////////////////////////////////////////////////*/
private     :
    // When attaching to windows, this is the original window procedure.
    LONG_PTR m_pOrigWndProc ;
    // Used for dialog box processing to determine if the
    // JWnd::DefWindowProc was called so the CommonDlgProc function can
    // set the dialog message result field.
    BOOL   m_bDefWndProcCalled ;

/*//////////////////////////////////////////////////////////////////////
                    Private Static Member Functions
//////////////////////////////////////////////////////////////////////*/
private     :
    // The windows message hook function used to set the this pointer
    // into the HWND long value.
    static LRESULT WINAPI CallWndProc ( int    nCode  ,
                                        WPARAM wParam ,
                                        LPARAM lParam  ) ;
    // The common windows procedure for all windows.
    static LRESULT WINAPI CommonWndProc ( HWND   hWnd   ,
                                          UINT   uMsg   ,
                                          WPARAM wParam ,
                                          LPARAM lParam  ) ;
    // The common dialog procedure for all dialogs.
    static BOOL WINAPI CommonDlgProc ( HWND   hWnd   ,
                                       UINT   uMsg   ,
                                       WPARAM wParam ,
                                       LPARAM lParam  ) ;

/*//////////////////////////////////////////////////////////////////////
                      Private Static Data Members
//////////////////////////////////////////////////////////////////////*/
private     :
    // The hook handle for the WH_CALLWNDPROC used to set the this
    // pointer into the window data.
    STATICTLS static HHOOK  sm_hhook ;
    // The this pointer the hook is supposed to set.
    STATICTLS static JWnd * sm_pThis ;

/*//////////////////////////////////////////////////////////////////////
                      Public Friends
//////////////////////////////////////////////////////////////////////*/
public      :
    friend BOOL JfxRegisterClass ( UINT      nClassStyle   ,
                                   LPTSTR    szClassName   ,
                                   UINT      nSizeOfName   ,
                                   HCURSOR   hCursor       ,
                                   HBRUSH    hbrBackground ,
                                   HICON     hIconBig      ,
                                   HICON     hIconSmall    ,
                                   HINSTANCE hInst          ) ;
} ;

/*//////////////////////////////////////////////////////////////////////
                            Global Functions
//////////////////////////////////////////////////////////////////////*/
// Registers a window class.  You can specify the class name in
// szClassName.  If the string is empty, the library will generate a
// name for you automagically.
BOOL JfxRegisterClass ( UINT      nClassStyle                      ,
                        LPTSTR    szClassName                      ,
                        UINT      nSizeOfName                      ,
                        HCURSOR   hCursor = NULL                   ,
                        HBRUSH    hbrBackGround = NULL             ,
                        HICON     hIconBig = NULL                  ,
                        HICON     hIconSmall = NULL                ,
                        HINSTANCE hInst = GetModuleHandle ( NULL )  ) ;

BOOL CopyTextToClipboard ( LPCTSTR szMem , HWND hWndOwner = NULL ) ;

/*//////////////////////////////////////////////////////////////////////
            Helper Macros for Easy Message Handler Declares
//////////////////////////////////////////////////////////////////////*/
#define BEGIN_MSG_DECLARES()                        \
    protected   :                                   \
    virtual LRESULT WindowProc ( UINT   uMsg   ,    \
                                 WPARAM wParam ,    \
                                 LPARAM lParam ) ;  \

#define END_MSG_DECLARES()

/*//////////////////////////////////////////////////////////////////////
Declare these macros inside the BEGIN_MSG_DECLARES...END_MSG_DECLARES
to specify the common messages your window class handles.
//////////////////////////////////////////////////////////////////////*/
#define DECL_WM_CREATE()        \
    virtual BOOL OnCreate ( LPCREATESTRUCT lpCreateStruct ) ;

#define DECL_WM_INITDIALOG()    \
    virtual BOOL OnInitDialog ( HWND hWndCtrl , LPARAM lParam ) ;

#define DECL_WM_CLOSE()         \
    virtual void OnClose ( void ) ;

#define DECL_WM_DESTROY()       \
    virtual void OnDestroy ( void ) ;

#define DECL_WM_ENDSESSION()    \
    virtual void OnEndSession ( BOOL bEnding , BOOL bLogOff ) ;

#define DECL_WM_QUERYENDSESSION()   \
    virtual BOOL OnQueryEndSession ( BOOL bLogOff ) ;

#define DECL_WM_PAINT()         \
    virtual void OnPaint ( void ) ;

#define DECL_WM_GETMINMAXINFO() \
    virtual int OnGetMinMaxInfo ( MINMAXINFO * lpMinMaxInfo ) ;

#define DECL_WM_SIZE()          \
    virtual void OnSize ( int iSizeReq , int iWidth , int iHeight ) ;

#define DECL_WM_INITMENU()      \
    virtual void OnInitMenu ( HMENU hMenu ) ;

#define DECL_WM_INITMENUPOPUP() \
    virtual void OnInitMenuPopup ( HMENU hMenu , int iPos , BOOL bWin );

#define DECL_WM_MENUSELECT()    \
    virtual BOOL OnMenuSelect ( int iCmd , int iFlags , HMENU hMenu ) ;

#define DECL_WM_SYSCOMMAND()    \
    virtual BOOL OnSysCommand ( int iCmdType ) ;

#define DECL_WM_NOTIFY()        \
    virtual LRESULT OnNotify ( int idFrom , NMHDR * pNMHDR ) ;

#define DECL_WM_DRAWITEM()      \
    virtual BOOL OnDrawItem ( UINT uiCtrl , LPDRAWITEMSTRUCT pDIS ) ;

#define DECL_WM_MEASUREITEM()      \
    virtual BOOL OnMeasureItem ( UINT                uiCtrl , \
                                 LPMEASUREITEMSTRUCT pMI     ) ;

#define DECL_WM_CTLCOLORBTN()   \
    virtual OnCtlColorBtn ( HDC hDC , HWND hWnd ) ;

#define DECL_WM_SETFOCUS()      \
    virtual BOOL OnSetFocus ( HWND hLostFocus ) ;

#define DECL_WM_KEYDOWN()       \
    virtual BOOL OnKeyDown ( DWORD dwVirtKey , DWORD dwKeyData ) ;

#define DECL_WM_KEYUP()         \
    virtual BOOL OnKeyUp ( DWORD dwVirtKey , DWORD dwKeyData ) ;

#define DECL_WM_LBUTTONDOWN()                             \
    virtual BOOL OnLeftButtonDown ( DWORD dwVirtKeys ,    \
                                    int   xPos       ,    \
                                    int   yPos        ) ;

#define DECL_WM_LBUTTONUP()                             \
    virtual BOOL OnLeftButtonUp ( DWORD dwVirtKeys ,    \
                                  int   xPos       ,    \
                                  int   yPos        ) ;

#define DECL_WM_RBUTTONDOWN()                              \
    virtual BOOL OnRightButtonDown ( DWORD dwVirtKeys ,    \
                                     int   xPos       ,    \
                                     int   yPos        ) ;

#define DECL_WM_RBUTTONUP()                              \
    virtual BOOL OnRightButtonUp ( DWORD dwVirtKeys ,    \
                                   int   xPos       ,    \
                                   int   yPos        ) ;

/*//////////////////////////////////////////////////////////////////////
The rest of the macros are for generic handling.  Each will have to be
declared in the BEGIN_MSG_MAP...END_MSG_MAP construct.
//////////////////////////////////////////////////////////////////////*/

// Generic message handlers (WM_TIMER, etc.)
#define DECL_MESSAGE_HANDLER(themsg,func)   \
    virtual LRESULT func ( WPARAM wParam , LPARAM lParam ) ;

// COMMAND_HANDLER functions.
#define DECL_COMMAND_HANDLER(id,code,func)  \
    virtual void func ( void ) ;

// COMMAND_ID_HANDLER functions.
#define DECL_COMMAND_ID_HANDLER(id,func)  \
    virtual void func ( void ) ;


// SIMPLE_COMMAND_ID_HANDLER function.
/*
#define DECL_SIMPLE_COMMAND_HANDLER(id,func)   \
    virtual void func ( void ) ;
*/

/*//////////////////////////////////////////////////////////////////////
Declare the BEGIN_MSG_MAP..END_MSG_MAP macros inside your CPP file.  The
handler macros build up your WindowProc procedure for the class.
//////////////////////////////////////////////////////////////////////*/
#define BEGIN_MSG_MAP(ThisClass,BaseClass)                          \
        LRESULT ThisClass :: WindowProc ( UINT   uMsg   ,           \
                                          WPARAM wParam ,           \
                                          LPARAM lParam )           \
        {                                                           \
            /* It needs to start with an if statement so the other*/\
            /* macros can use if else.                            */\
            if ( 0 == uMsg )                                        \
            {                                                       \
                return ( 0 ) ;                                      \
            }


#define END_MSG_MAP(ThisClass,BaseClass)                            \
            return ( BaseClass::WindowProc ( uMsg    ,              \
                                             wParam  ,              \
                                             lParam   ) ) ;         \
        }

/*//////////////////////////////////////////////////////////////////////
The common but not that common message handlers for the
BEGIN_MSG_MAP...END_MSG_MAP construct.
//////////////////////////////////////////////////////////////////////*/

#define WM_CREATE_HANDLER()                                            \
            else if ( WM_CREATE == uMsg )                              \
            {                                                          \
                return ( OnCreate ( (LPCREATESTRUCT)lParam ) ) ;       \
            }

#define WM_INITDIALOG_HANDLER()                                        \
            else if ( WM_INITDIALOG == uMsg )                          \
            {                                                          \
                return ( OnInitDialog ( (HWND)wParam , lParam ) ) ;    \
            }

#define WM_CLOSE_HANDLER()                                             \
            else if ( WM_CLOSE == uMsg )                               \
            {                                                          \
                OnClose ( ) ;                                          \
                return ( 0 ) ;                                         \
            }

#define WM_DESTROY_HANDLER()                                           \
            else if ( WM_DESTROY == uMsg )                             \
            {                                                          \
                OnDestroy ( ) ;                                        \
                return ( 0 ) ;                                         \
            }

#define WM_ENDSESSION_HANDLER()                                        \
            else if ( WM_ENDSESSION == uMsg )                          \
            {                                                          \
                OnEndSession ( (BOOL)wParam , 0 == lParam ) ;          \
                return ( 0 ) ;                                         \
            }

#define WM_QUERYENDSESSION_HANDLER()                                   \
            else if ( WM_QUERYENDSESSION == uMsg )                     \
            {                                                          \
                return ( OnQueryEndSession ( 0 == lParam ) ) ;         \
            }

#define WM_PAINT_HANDLER()                                             \
            else if ( WM_PAINT == uMsg )                               \
            {                                                          \
                OnPaint ( ) ;                                          \
                return ( 0 ) ;                                         \
            }

#define WM_GETMINMAXINFO_HANDLER()                                     \
            else if ( WM_GETMINMAXINFO == uMsg )                       \
            {                                                          \
                return ( OnGetMinMaxInfo ( (MINMAXINFO*)lParam ) ) ;   \
            }

#define WM_SIZE_HANDLER()                                              \
            else if ( WM_SIZE == uMsg )                                \
            {                                                          \
                OnSize ( (int)wParam        ,                          \
                          LOWORD ( lParam ) ,                          \
                          HIWORD ( lParam )  ) ;                       \
                return ( 0 ) ;                                         \
            }

#define WM_INITMENU_HANDLER()                                          \
            else if ( WM_INITMENU == uMsg )                            \
            {                                                          \
                OnInitMenu ( (HMENU)wParam ) ;                         \
                return ( 0 ) ;                                         \
            }
#define WM_INITMENUPOPUP_HANDLER()                                     \
            else if ( WM_INITMENUPOPUP == uMsg )                       \
            {                                                          \
                OnInitMenuPopup ( (HMENU)wParam      ,                 \
                                   LOWORD ( lParam ) ,                 \
                                   HIWORD ( lParam )  ) ;              \
                return ( 0 ) ;                                         \
            }

#define WM_MENUSELECT_HANDLER()                                        \
            else if ( WM_MENUSELECT == uMsg )                          \
            {                                                          \
                return(OnMenuSelect(GET_WM_MENUSELECT_CMD   ( wParam , \
                                                              lParam), \
                                    GET_WM_MENUSELECT_FLAGS ( wParam , \
                                                              lParam), \
                                    GET_WM_MENUSELECT_HMENU ( wParam,  \
                                                            lParam))); \
            }

#define WM_SYSCOMMAND_HANDLER()                                        \
            else if ( WM_SYSCOMMAND == uMsg )                          \
            {                                                          \
                return ( OnSysCommand ( (int)wParam ) ) ;              \
            }

#define WM_NOTIFY_HANDLER()                                            \
            else if ( WM_NOTIFY == uMsg )                              \
            {                                                          \
                return ( OnNotify ( (int)wParam , (NMHDR*)lParam ) ) ; \
            }

#define WM_DRAWITEM_HANDLER()                                          \
            else if ( WM_DRAWITEM == uMsg )                            \
            {                                                          \
                return ( OnDrawItem ( (UINT)wParam             ,       \
                                      (LPDRAWITEMSTRUCT)lParam  ) ) ;  \
            }

#define WM_MEASUREITEM_HANDLER()                                       \
            else if ( WM_MEASUREITEM == uMsg )                         \
            {                                                          \
                return ( OnMeasureItem ((UINT)wParam               ,   \
                                        (LPMEASUREITEMSTRUCT)lParam)); \
            }


#define WM_CTLCOLORBTN_HANDLER()                                       \
            else if ( WM_CTLCOLORBTN == uMsg )                         \
            {                                                          \
                return ( OnCtlColorBtn ( (HDC)wParam  ,                \
                                         (HWND)lParam  ) ) ;           \
            }


#define WM_SETFOCUS_HANDLER()                                          \
            else if ( WM_SETFOCUS == uMsg )                            \
            {                                                          \
                return ( OnSetFocus ( (HWND)wParam ) ) ;               \
            }

#define WM_KEYDOWN_HANDLER()                                           \
            else if ( WM_KEYDOWN == uMsg )                             \
            {                                                          \
                return ( OnKeyDown ( (DWORD)wParam , (DWORD)lParam )); \
            }

#define WM_KEYUP_HANDLER()                                             \
            else if ( WM_KEYUP == uMsg )                               \
            {                                                          \
                return ( OnKeyUp ( (DWORD)wParam , (DWORD)lParam ) );  \
            }

#define WM_LBUTTONDOWN_HANDLER()                                       \
            else if ( WM_LBUTTONDOWN == uMsg )                         \
            {                                                          \
                return ( OnLeftButtonDown ( (DWORD)wParam          ,   \
                                             GET_X_LPARAM(lParam)  ,   \
                                             GET_Y_LPARAM(lParam))  ); \
            }

#define WM_LBUTTONUP_HANDLER()                                         \
            else if ( WM_LBUTTONUP == uMsg )                           \
            {                                                          \
                return ( OnLeftButtonUp ( (DWORD)wParam          ,     \
                                           GET_X_LPARAM(lParam)  ,     \
                                           GET_Y_LPARAM(lParam))  );   \
            }

#define WM_RBUTTONDOWN_HANDLER()                                       \
            else if ( WM_RBUTTONDOWN == uMsg )                         \
            {                                                          \
                return ( OnRightButtonDown ((DWORD)wParam          ,   \
                                             GET_X_LPARAM(lParam)  ,   \
                                             GET_Y_LPARAM(lParam))  ); \
            }

#define WM_RBUTTONUP_HANDLER()                                         \
            else if ( WM_RBUTTONUP == uMsg )                           \
            {                                                          \
                return ( OnRightButtonUp ( (DWORD)wParam         ,     \
                                           GET_X_LPARAM(lParam)  ,     \
                                           GET_Y_LPARAM(lParam))  ) ;  \
            }

/*//////////////////////////////////////////////////////////////////////
Generic message and command handling.
//////////////////////////////////////////////////////////////////////*/

// Use the DECL_MESSAGE_HANDLER(func) macro above to declare.
// Define your message handlers like the following:
// LRESULT MesageHandler ( WPARAM wParam , LPARAM lParam )
#define MESSAGE_HANDLER(themsg , func)                  \
            else if ( uMsg == themsg )                  \
            {                                           \
                return ( func ( wParam ,                \
                                lParam  ) ) ;           \
            }

// Use the DECL_COMMAND_HANDLER(func) above to declare.
// Define your command handlers for COMMAND_HANDLER and
// COMMAND_ID_HANDLER like the following:
// void CommandHandler ( void )
#define COMMAND_HANDLER(id , code , func)                   \
            else if ( ( WM_COMMAND        == uMsg ) &&      \
                      ( LOWORD ( wParam ) == id   ) &&      \
                      ( HIWORD ( wParam ) == code )    )    \
            {                                               \
                func ( ) ;                                  \
                return ( 0 ) ;                              \
            }

#define COMMAND_ID_HANDLER(id , func)                   \
            else if ( ( WM_COMMAND == uMsg      ) &&    \
                      ( id == LOWORD ( wParam ) )   )   \
            {                                           \
                func ( ) ;                              \
                return ( 0 ) ;                          \
            }

// Use DECL_SIMPLE_COMMAND_HANDLER to declare.
// Define your command handlers for SIMPLE_COMMAND_ID_HANDLER like
// the following:
// void CommandHandler ( void ) ;
/*
#define SIMPLE_COMMAND_ID_HANDLER(id , func)            \
            else if ( ( WM_COMMAND == uMsg      ) &&    \
                      ( id == LOWORD ( wParam ) )    )  \
            {                                           \
                func ( ) ;                              \
                return ( 0 ) ;                          \
            }
*/

/*//////////////////////////////////////////////////////////////////////
Macros for doing default (DefWindowProc) processing on messages where
the base class does not define a message handler.  In other words, these
defines are best used when subclassing a HWND-based control.
//////////////////////////////////////////////////////////////////////*/
#define DEF_WM_CREATE_HANDLING(lpCreateStruct)                         \
            DefWindowProc ( WM_CREATE , 0 , (LPARAM)lpCreateStruct )

#define DEF_WM_INITDIALOG_HANDLING(hWndCtrl,lParam)                    \
            DefWindowProc ( WM_INITIDIALOG   ,                         \
                            (WPARAM)hWndCtrl ,                         \
                            lParam )

#define DEF_WM_CLOSE_HANDLING()                                        \
            DefWindowProc ( WM_CLOSE , 0 , 0 )

#define DEF_WM_DESTROY_HANDLING()                                      \
            DefWindowProc ( WM_DESTROY , 0 , 0 )

#define DEF_WM_ENDSESSION_HANDLING(bEnding,bLogOff)                    \
            DefWindowProc ( WM_ENDSESSION       ,                      \
                            (WPARAM)bEnding     ,                      \
                            m_lParam             )

#define DEF_WM_QUERYENDSESSION_HANDLING(bLogOff)                       \
            DefWindowProc ( WM_QUERYENDSESSION , 0 , m_lParam )

#define DEF_WM_PAINT_HANDLING()                                        \
            DefWindowProc ( WM_PAINT , 0 , 0 )

#define DEF_WM_GETMINMAXINFO_HANDLING(lpMinMaxInfo)                    \
            DefWindowProc ( WM_GETMINMAXINFO     ,                     \
                            0                    ,                     \
                            (LPARAM)lpMinMaxInfo  )

#define DEF_WM_SIZE_HANDLING(iSizeReq,iWidth,iHeight)                  \
            DefWindowProc ( WM_SIZE                     ,              \
                            (WPARAM)iSizeReq            ,              \
                            MAKELONG(iWidth,iHeight)     )

#define DEF_WM_INITMENU_HANDLING(hMenu)                                \
            DefWindowProc ( WM_INITMENU , (WPARAM)hMenu , 0 )

#define DEF_WM_INITMENUPOPUP_HANDLING(hMenu,iPos,bWin)                 \
            DefWindowProc ( WM_INITMENUPOPUP        ,                  \
                            (WPARAM)hMenu           ,                  \
                            MAKELONG(iPos , bWin )   )

#define DEF_WM_MENUSELECT_HANDLING(iCmd,iFlags,hMenu)                  \
            DefWindowProc ( WM_MENUSELECT                   ,          \
                            (WPARAM)MAKELONG(iCmd,iFlags)   ,          \
                            (LPARAM)hMenu                    )

#define DEF_WM_SYSCOMMAND_HANDLING(iCmdType)                           \
            DefWindowProc ( WM_SYSCOMMAND       ,                      \
                            (WPARAM)iCmdType    ,                      \
                            m_lParam             )

#define DEF_WM_NOTIFY_HANDLING(idFrom,pNMHDR)                          \
            DefWindowProc ( WM_NOTIFY     ,                            \
                           (WPARAM)idFrom ,                            \
                           (LPARAM)pNMHDR  )

#define DEF_WM_DRAWITEM_HANDLING(uiCtrl,pDIS)                          \
            DefWindowProc ( WM_DRAWITEM     ,                          \
                            (WPARAM)uiCtrl  ,                          \
                            (LPARAM)pDis     )

#define DEF_WM_CTLCOLORBTN_HANDLING(hDC,hWnd)                          \
            DefWindowProc ( WM_CTLCOLORBTN      ,                      \
                            (WPARAM)hDC         ,                      \
                            (LPARAM)hWnd         )

#define DEF_WM_SETFOCUS_HANDLING(hLostFocus)                           \
            DefWindowProc ( WM_SETFOCUS , (WPARAM)hLostFocus , 0 )

#define DEF_WM_KEYDOWN_HANDLING(dwVirtKey,dwKeyData)                   \
            DefWindowProc ( WM_KEYDOWN          ,                      \
                            (WPARAM)dwVirtKey   ,                      \
                            (LPARAM)dwKeyData     )

#define DEF_WM_KEYUP_HANDLING(dwVirtKey,dwKeyData)                     \
            DefWindowProc ( WM_KEYUP            ,                      \
                            (WPARAM)dwVirtKey   ,                      \
                            (LPARAM)dwKeyData     )

#define DEF_WM_LBUTTONDOWN_HANDLING(dwVirtKeys,xPos,yPos)              \
            DefWindowProc ( WM_LBUTTONDOWN         ,                   \
                            (WPARAM)dwVirtKeys     ,                   \
                            MAKELONG ( xPos,yPos )  )

#define DEF_WM_LBUTTONUP_HANDLING(dwVirtKeys,xPos,yPos)                \
            DefWindowProc ( WM_LBUTTONUP           ,                   \
                            (WPARAM)dwVirtKeys     ,                   \
                            MAKELONG ( xPos,yPos )  )

#define DEF_WM_RBUTTONDOWN_HANDLING(dwVirtKeys,xPos,yPos)              \
            DefWindowProc ( WM_RBUTTONDOWN         ,                   \
                            (WPARAM)dwVirtKeys     ,                   \
                            MAKELONG ( xPos,yPos )  )

#define DEF_WM_RBUTTONUP_HANDLING(dwVirtKeys,xPos,yPos)                \
            DefWindowProc ( WM_RBUTTONUP           ,                   \
                            (WPARAM)dwVirtKeys     ,                   \
                            MAKELONG ( xPos,yPos )  )

#define DEF_MESSAGE_HANDLING(themsg,wPar,lPar)                         \
            DefWindowProc ( themsg  , wPar , lPar )

#define DEF_WM_COMMAND_HANDLING(wNotifyCode,wID,hWndCtl)               \
            DefWindowProc ( WM_COMMAND                     ,           \
                            MAKELONG ( wID , wNotifyCode ) ,           \
                            (LPARAM)hWndCtl                 )
#endif  // _JWND_H

