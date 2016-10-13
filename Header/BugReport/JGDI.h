/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JGDI_H
#define _JGDI_H

// The simple GDI object class
class JGdiObject
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JGdiObject ( void )
    {
        m_hObject = NULL ;
        m_bDelete = FALSE ;
    }

    JGdiObject ( HGDIOBJ hObj , BOOL bAutoDelete = TRUE )
    {
        ASSERT ( NULL != hObj ) ;
        m_hObject = hObj ;
        m_bDelete = bAutoDelete ;
    }

    virtual ~JGdiObject ( void )
    {
        if ( ( TRUE == m_bDelete ) && ( NULL != m_hObject ) )
        {
            DeleteObject ( ) ;
        }
    }

/*//////////////////////////////////////////////////////////////////////
                            Public Operators
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HGDIOBJ ( ) const
    {
        return ( m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Helper Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    BOOL Attach ( HGDIOBJ hObj , BOOL bAutoDelete = TRUE )
    {
        ASSERT ( NULL == m_hObject ) ;
        m_hObject = hObj ;
        m_bDelete = bAutoDelete ;
        return ( TRUE ) ;
    }

    HGDIOBJ Detach ( void )
    {
        HGDIOBJ hRet = m_hObject ;
        m_hObject = NULL ;
        m_bDelete = FALSE ;
        return ( hRet ) ;
    }

    DWORD GetObjectType ( void )
    {
        ASSERT ( NULL != m_hObject ) ;
        return ( ::GetObjectType ( m_hObject ) ) ;
    }

    BOOL DeleteObject ( void )
    {
        ASSERT ( NULL != m_hObject ) ;
        BOOL bRet = ::DeleteObject ( Detach ( ) ) ;
        ASSERT ( TRUE == bRet ) ;
        return ( bRet ) ;
    }


/*//////////////////////////////////////////////////////////////////////
                         Protected Data Members
//////////////////////////////////////////////////////////////////////*/
protected   :
    HGDIOBJ m_hObject ;
    BOOL m_bDelete ;
} ;


class JPen : public JGdiObject
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JPen ( void )
    {
    }

    JPen ( HGDIOBJ hObj , BOOL bAutoDelete )
        : JGdiObject ( hObj , bAutoDelete )
    {
        ASSERT ( NULL != hObj ) ;
    }

    JPen ( int fnPenStyle , int nWidth , COLORREF crColor )
    {
        VERIFY ( CreatePen ( fnPenStyle , nWidth , crColor ) ) ;
        m_bDelete = TRUE ;
    }

    virtual ~JPen ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                            Public Operators
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HPEN ( ) const
    {
        return ( (HPEN)m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Helper Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    BOOL CreatePen ( int fnPenStyle , int nWidth , COLORREF crColor )
    {
        ASSERT  ( NULL == m_hObject ) ;
        m_hObject = (HGDIOBJ)::CreatePen ( fnPenStyle ,
                                           nWidth     ,
                                           crColor     ) ;
        m_bDelete = TRUE ;
        ASSERT ( NULL != m_hObject ) ;
        return ( NULL != m_hObject ) ;
    }

    BOOL CreatePenIndirect ( const LOGPEN * lplgpn )
    {
        ASSERT  ( NULL == m_hObject ) ;
        m_hObject = (HGDIOBJ)::CreatePenIndirect ( lplgpn ) ;
        m_bDelete = TRUE ;
        ASSERT ( NULL != m_hObject ) ;
        return ( NULL != m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Protected Data Members
//////////////////////////////////////////////////////////////////////*/
protected   :
} ;


class JBitmap : public JGdiObject
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JBitmap ( void )
    {
    }

    JBitmap ( HGDIOBJ hObj , BOOL bAutoDelete = TRUE )
        : JGdiObject ( hObj , bAutoDelete )
    {
        ASSERT ( NULL != hObj ) ;
    }


    JBitmap ( int  nWidth     ,
              int  nHeight    ,
              UINT nPlanes    ,
              UINT nBitcount  ,
              const void* lpBits  = NULL )
    {
        VERIFY ( CreateBitmap ( nWidth     ,
                                nHeight    ,
                                nPlanes    ,
                                nBitcount  ,
                                lpBits      ) ) ;
        m_bDelete = TRUE ;
    }

    virtual ~JBitmap ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                            Public Operators
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HBITMAP ( ) const
    {
        return ( (HBITMAP)m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Helper Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    BOOL CreateBitmap ( int  nWidth     ,
                        int  nHeight    ,
                        UINT nPlanes    ,
                        UINT nBitcount  ,
                        const void* lpBits  = NULL )
    {
        ASSERT ( NULL == m_hObject ) ;
        m_hObject = ::CreateBitmap ( nWidth     ,
                                     nHeight    ,
                                     nPlanes    ,
                                     nBitcount  ,
                                     lpBits      ) ;
        m_bDelete = TRUE ;
        ASSERT ( NULL != m_hObject ) ;
        return ( NULL != m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Protected Data Members
//////////////////////////////////////////////////////////////////////*/
protected   :
} ;

class JFont : public JGdiObject
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JFont ( void )
    {
    }

    JFont ( HGDIOBJ hObj , BOOL bAutoDelete )
        : JGdiObject ( hObj , bAutoDelete )
    {
        ASSERT ( NULL != hObj ) ;
    }

    JFont ( int      nPointSize      ,
            LPCTSTR  szFaceName      ,
            HDC      hDC = NULL      ,
            BOOL     bItalic = FALSE ,
            BOOL     bBold = FALSE    )
    {
        VERIFY ( CreatePointFont ( nPointSize ,
                                   szFaceName ,
                                   hDC        ,
                                   bItalic    ,
                                   bBold       ) ) ;
        m_bDelete = TRUE ;
    }

    virtual ~JFont ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                            Public Operators
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HFONT ( ) const
    {
        return ( (HFONT)m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Helper Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    // Pass in the real point size.  If you want a 12 point font,
    // pass in "12".
    BOOL CreatePointFont ( int      nPointSize      ,
                           LPCTSTR  szFaceName      ,
                           HDC      hDC = NULL      ,
                           BOOL     bItalic = FALSE ,
                           BOOL     bBold = FALSE    ) ;

    HFONT CreateFontIndirect ( CONST LOGFONT * lpLF )
    {
        m_hObject = ::CreateFontIndirect ( lpLF ) ;
        m_bDelete = TRUE ;
        ASSERT ( NULL != m_hObject ) ;
        return ( (HFONT)m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Protected Data Members
//////////////////////////////////////////////////////////////////////*/
protected   :
} ;

class JBrush : public JGdiObject
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JBrush ( void )
    {
    }

    JBrush ( HGDIOBJ hObj , BOOL bAutoDelete )
        : JGdiObject ( hObj , bAutoDelete )
    {
        ASSERT ( NULL != hObj ) ;
    }

    JBrush ( COLORREF crColor )
    {
        VERIFY ( CreateSolidBrush ( crColor ) ) ;
        m_bDelete = TRUE ;
    }

    virtual ~JBrush ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
                            Public Operators
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HBRUSH ( ) const
    {
        return ( (HBRUSH)m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Helper Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    BOOL CreateSolidBrush ( COLORREF crColor )
    {
        ASSERT ( NULL == m_hObject ) ;
        m_hObject = ::CreateSolidBrush ( crColor ) ;
        m_bDelete = TRUE ;
        ASSERT ( NULL != m_hObject ) ;
        return ( NULL != m_hObject ) ;
    }

    BOOL CreateHatchBrush ( int fnStyle , COLORREF clrref )
    {
        ASSERT ( NULL == m_hObject ) ;
        m_hObject = ::CreateHatchBrush ( fnStyle , clrref ) ;
        m_bDelete = TRUE ;
        ASSERT ( NULL != m_hObject ) ;
        return ( NULL != m_hObject ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Protected Data Members
//////////////////////////////////////////////////////////////////////*/
protected   :
} ;


class JDC
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :

    JDC ( void )
    {
        m_hDC = NULL ;
        m_bDelete = FALSE ;
    }

    JDC ( HDC hDC , BOOL bAutoDelete = TRUE )
    {
        m_hDC = NULL ;
        m_bDelete = FALSE ;
        Attach ( hDC , bAutoDelete ) ;
    }

    virtual ~JDC ( void )
    {
        if ( ( TRUE == m_bDelete ) && ( NULL != m_hDC ) )
        {
            DeleteDC ( ) ;
        }
    }

/*//////////////////////////////////////////////////////////////////////
                            Public Operators
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HDC ( ) const
    {
        return ( m_hDC ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Helper Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    BOOL Attach ( HDC hDC , BOOL bAutoDelete = TRUE )
    {
        ASSERT ( NULL == m_hDC ) ;
        m_hDC = hDC ;
        m_bDelete = bAutoDelete ;
        return ( TRUE ) ;
    }

    HDC Detach ( void )
    {
        HDC hRet = m_hDC ;
        m_hDC = NULL ;
        m_bDelete = FALSE ;
        return ( hRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                   Public Creation & Deletion Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    BOOL CreateCompatibleDC ( JDC & rDC )
    {
        ASSERT ( NULL == m_hDC ) ;
        if ( NULL != m_hDC )
        {
            return ( FALSE ) ;
        }
        m_hDC = ::CreateCompatibleDC ( rDC.m_hDC ) ;
        ASSERT ( NULL != m_hDC ) ;
        m_bDelete = TRUE ;
        return ( NULL != m_hDC ) ;
    }

    BOOL CreateCompatibleDC ( HDC hDC )
    {
        ASSERT ( NULL == m_hDC ) ;
        if ( NULL != m_hDC )
        {
            return ( FALSE ) ;
        }
        m_hDC = ::CreateCompatibleDC ( hDC ) ;
        ASSERT ( NULL != m_hDC ) ;
        m_bDelete = TRUE ;
        return ( NULL != m_hDC ) ;
    }

    BOOL DeleteDC ( void )
    {
        ASSERT ( NULL != m_hDC ) ;
        BOOL bRet = ::DeleteDC ( Detach ( ) ) ;
        ASSERT ( TRUE == bRet ) ;
        return ( bRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                        Public Selection Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    HPEN SelectObject ( const JPen & rPen )
    {
        ASSERT ( NULL != m_hDC ) ;
        HGDIOBJ hObj = ::SelectObject ( m_hDC , (HGDIOBJ)rPen ) ;
        ASSERT ( NULL != hObj ) ;
        return ( (HPEN)hObj ) ;
    }

    HPEN SelectObject ( HPEN hPen )
    {
        ASSERT ( NULL != m_hDC ) ;
        HGDIOBJ hObj = ::SelectObject ( m_hDC , (HGDIOBJ)hPen ) ;
        ASSERT ( NULL != hObj ) ;
        return ( (HPEN)hObj ) ;
    }

    HBITMAP SelectObject ( const JBitmap & rBitmap )
    {
        ASSERT ( NULL != m_hDC ) ;
        HGDIOBJ hObj = ::SelectObject ( m_hDC , (HGDIOBJ)rBitmap ) ;
        ASSERT ( NULL != hObj ) ;
        return ( (HBITMAP)hObj ) ;
    }

    HBITMAP SelectObject ( HBITMAP hBitmap )
    {
        ASSERT ( NULL != m_hDC ) ;
        HGDIOBJ hObj = ::SelectObject ( m_hDC , (HGDIOBJ)hBitmap ) ;
        ASSERT ( NULL != hObj ) ;
        return ( (HBITMAP)hObj ) ;
    }

    HFONT SelectObject ( const JFont & rFont )
    {
        ASSERT ( NULL != m_hDC ) ;
        HGDIOBJ hObj = ::SelectObject ( m_hDC , (HGDIOBJ)rFont ) ;
        ASSERT ( NULL != hObj ) ;
        return ( (HFONT)hObj ) ;
    }

    HFONT SelectObject ( HFONT hFont )
    {
        ASSERT ( NULL != m_hDC ) ;
        HGDIOBJ hObj = ::SelectObject ( m_hDC , (HGDIOBJ)hFont ) ;
        ASSERT ( NULL != hObj ) ;
        return ( (HFONT)hObj ) ;
    }

    HGDIOBJ SelectObject ( HGDIOBJ hInObj )
    {
        ASSERT ( NULL != m_hDC ) ;
        HGDIOBJ hObj = ::SelectObject ( m_hDC , hInObj ) ;
        ASSERT ( NULL != hObj ) ;
        return ( hObj ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                        Public Gets/Sets Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    BOOL GetTextMetrics ( LPTEXTMETRIC lpTM )
    {
        ASSERT ( NULL != m_hDC ) ;
        return ( ::GetTextMetrics ( m_hDC , lpTM ) ) ;
    }

    int SetBkMode ( int iBkMode )
    {
        ASSERT ( NULL != m_hDC ) ;
        return ( ::SetBkMode ( m_hDC , iBkMode ) ) ;
    }

    COLORREF SetBkColor ( COLORREF crColor )
    {
        ASSERT ( NULL != m_hDC ) ;
        return( ::SetBkColor ( m_hDC , crColor ) ) ;
    }

    COLORREF SetTextColor ( COLORREF clrText )
    {
        ASSERT ( NULL != m_hDC ) ;
        return ( ::SetTextColor( m_hDC , clrText ) ) ;
    }

    UINT SetTextAlign ( UINT fMode )
    {
        ASSERT ( NULL != m_hDC ) ;
        return ( ::SetTextAlign ( m_hDC , fMode ) ) ;
    }


/*//////////////////////////////////////////////////////////////////////
                       Public Device Info Methods
//////////////////////////////////////////////////////////////////////*/
public      :

    int GetDeviceCaps ( int nIndex )
    {
        ASSERT ( NULL != m_hDC ) ;
        return ( ::GetDeviceCaps ( m_hDC , nIndex ) ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                    Public Drawing/Blitting Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    JPoint MoveTo ( int X , int Y )
    {
        ASSERT ( NULL != m_hDC ) ;
        JPoint pt ;
        VERIFY ( ::MoveToEx ( m_hDC , X , Y , &pt ) ) ;
        return ( pt ) ;
    }

    JPoint MoveTo ( JPoint & ptIN )
    {
        ASSERT ( NULL != m_hDC ) ;
        JPoint pt ;
        VERIFY ( ::MoveToEx ( m_hDC , ptIN.x  , ptIN.y , &pt ) ) ;
        return ( pt ) ;
    }


    BOOL LineTo ( int X , int Y )
    {
        ASSERT ( NULL != m_hDC ) ;
        return ( ::LineTo ( m_hDC , X , Y ) ) ;
    }

    BOOL LineTo ( JPoint pt )
    {
        ASSERT ( NULL != m_hDC ) ;
        return ( ::LineTo ( m_hDC , pt.x , pt.y ) ) ;
    }

    BOOL FillRect ( JRect * lpRect , HBRUSH hBr )
    {
        ASSERT ( NULL != m_hDC ) ;
        BOOL bRet = ::FillRect ( m_hDC , lpRect , hBr ) ;
        ASSERT ( bRet ) ;
        return ( bRet != 0 ) ;
    }

    BOOL FillRect ( RECT * lpRect , HBRUSH hBr )
    {
        ASSERT ( NULL != m_hDC ) ;
        BOOL bRet = ::FillRect ( m_hDC , lpRect , hBr ) ;
        ASSERT ( bRet ) ;
        return ( bRet != 0 ) ;
    }

    BOOL BitBlt ( int   nXDest              ,
                  int   nYDest              ,
                  int   nWidth              ,
                  int   nHeight             ,
                  HDC   hdcSrc              ,
                  int   nXSrc    = 0        ,
                  int   nYSrc    = 0        ,
                  DWORD dwRop    = SRCCOPY    )
    {
        ASSERT ( NULL != m_hDC ) ;
        BOOL bRet = ::BitBlt ( m_hDC    ,
                               nXDest   ,
                               nYDest   ,
                               nWidth   ,
                               nHeight  ,
                               hdcSrc   ,
                               nXSrc    ,
                               nYSrc    ,
                               dwRop     ) ;
        ASSERT ( bRet ) ;
        return ( bRet ) ;
    }

    BOOL Rectangle ( int nLeftRect   ,
                     int nTopRect    ,
                     int nRightRect  ,
                     int nBottomRect  )
    {
        ASSERT ( NULL != m_hDC ) ;
        BOOL bRet = ::Rectangle ( m_hDC       ,
                                  nLeftRect   ,
                                  nTopRect    ,
                                  nRightRect  ,
                                  nBottomRect  ) ;
        ASSERT ( bRet ) ;
        return ( bRet ) ;
    }

    int DrawText ( LPCTSTR lpString ,
                   LPRECT  lpRect   ,
                   UINT    uFormat   )
    {
        ASSERT ( NULL != m_hDC ) ;
        int iRet = ::DrawText ( m_hDC       ,
                                lpString    ,
                                -1          ,
                                lpRect      ,
                                uFormat      ) ;
        ASSERT ( iRet ) ;
        return ( iRet ) ;
    }

    int DrawTextEx ( LPTSTR           lpchText           ,
                     int              cchText            ,
                     LPRECT           lprc               ,
                     UINT             dwDTFormat         ,
                     LPDRAWTEXTPARAMS lpDTParams  = NULL  )
    {
        ASSERT ( NULL != m_hDC ) ;
        int iRet = ::DrawTextEx ( m_hDC      ,
                                  lpchText   ,
                                  cchText    ,
                                  lprc       ,
                                  dwDTFormat ,
                                  lpDTParams  ) ;
        ASSERT ( iRet ) ;
        return ( iRet ) ;
    }


    BOOL TextOut ( int     nXStart      ,
                   int     nYStart      ,
                   LPCTSTR lpString     ,
                   int     cbString = 0  )
    {
        ASSERT ( NULL != m_hDC ) ;
        if ( 0 == cbString )
        {
            cbString = lstrlen ( lpString ) ;
        }
        BOOL bRet = ::TextOut ( m_hDC    ,
                                nXStart  ,
                                nYStart  ,
                                lpString ,
                                cbString  ) ;
        ASSERT ( bRet ) ;
        return ( bRet ) ;
    }

    BOOL DrawIconEx ( int       xLeft               ,
                      int       yTop                ,
                      HICON     hIcon               ,
                      int       cxWidth             ,
                      int       cyWidth             ,
                      UINT      istepIfAniCur       ,
                      HBRUSH    hbrFlickerFreeDraw  ,
                      UINT      diFlags              )
    {
        ASSERT ( NULL != m_hDC ) ;
        BOOL bRet = ::DrawIconEx ( m_hDC               ,
                                   xLeft               ,
                                   yTop                ,
                                   hIcon               ,
                                   cxWidth             ,
                                   cyWidth             ,
                                   istepIfAniCur       ,
                                   hbrFlickerFreeDraw  ,
                                   diFlags              ) ;
        ASSERT ( bRet ) ;
        return ( bRet ) ;
    }

    BOOL DrawFocusRect ( CONST RECT * lpRC )
    {
        ASSERT ( NULL != m_hDC ) ;
        BOOL bRet = ::DrawFocusRect ( m_hDC , lpRC ) ;
        ASSERT ( bRet ) ;
        return ( bRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Protected Data Members
//////////////////////////////////////////////////////////////////////*/
protected   :
    HDC m_hDC ;
    BOOL m_bDelete ;
} ;

#endif _JGDI_H