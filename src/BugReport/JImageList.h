/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JIMAGELIST_H
#define _JIMAGELIST_H

class JImageList
{
/*//////////////////////////////////////////////////////////////////////
                  Public Constructors and Destructors
//////////////////////////////////////////////////////////////////////*/
public      :
    JImageList ( void )
    {
        m_hIML = NULL ;
        m_bDelete = FALSE ;
    }

    virtual ~JImageList ( void )
    {
        if ( ( TRUE == m_bDelete ) && ( NULL != m_hIML ) )
        {
            Destroy ( ) ;
        }
    }

/*//////////////////////////////////////////////////////////////////////
                            Public Operators
//////////////////////////////////////////////////////////////////////*/
public      :
    operator HIMAGELIST ( ) const
    {
        return ( m_hIML ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                         Public Helper Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    BOOL Attach ( HIMAGELIST hIML , BOOL bAutoDelete = TRUE )
    {
        ASSERT ( NULL == hIML ) ;
        m_hIML = hIML ;
        m_bDelete = bAutoDelete ;
        return ( TRUE ) ;
    }

    HIMAGELIST Detach ( void )
    {
        HIMAGELIST hRet = m_hIML ;
        m_hIML = NULL ;
        m_bDelete = FALSE ;
        return ( hRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                            Public Wrapper City
//////////////////////////////////////////////////////////////////////*/
public      :

    int Add ( HBITMAP hbmImage , HBITMAP hbmMask = NULL )
    {
        ASSERT ( NULL != m_hIML ) ;
        int iRet = ImageList_Add ( m_hIML , hbmImage , hbmMask ) ;
        ASSERT ( -1 != iRet ) ;
        return ( iRet ) ;
    }

    int AddMasked ( HBITMAP hbmImage , COLORREF crMask )
    {
        ASSERT ( NULL != m_hIML ) ;
        int iRet = ImageList_AddMasked ( m_hIML , hbmImage , crMask ) ;
        ASSERT ( -1 != iRet ) ;
        return ( iRet ) ;
    }

    int AddIcon ( HICON hIcon )
    {
        ASSERT ( NULL != m_hIML ) ;
        int iRet = ImageList_AddIcon ( m_hIML , hIcon ) ;
        ASSERT ( -1 != iRet ) ;
        return ( iRet ) ;
    }

    BOOL Create ( int  cx                   ,
                  int  cy                   ,
                  UINT flags = ILC_MASK     ,
                  int  cInitial = 1         ,
                  int  cGrow = 5             )
    {
        ASSERT ( NULL == m_hIML ) ;
        m_hIML = ImageList_Create( cx , cy , flags , cInitial , cGrow);
        ASSERT ( NULL != m_hIML ) ;
        m_bDelete = TRUE ;
        return ( NULL != m_hIML ) ;
    }

    BOOL Destroy ( void )
    {
        ASSERT ( NULL != m_hIML ) ;
        BOOL bRet = ::ImageList_Destroy ( m_hIML ) ;
        ASSERT ( FALSE != bRet ) ;
        return ( bRet ) ;
    }

/*//////////////////////////////////////////////////////////////////////
                        Private Data Methods
//////////////////////////////////////////////////////////////////////*/
private :
    // The imagelist itself.
    HIMAGELIST m_hIML ;
    // Destroy or not to destroy.
    BOOL m_bDelete ;
} ;

#endif  // _JIMAGELIST_H


