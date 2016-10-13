/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
/*----------------------------------------------------------------------
    A simple critical section wrapper that makes it easy to use a
critical section.  Just declare a single CCriticalSection as part of
your class data and whenever you need to user it, use the
CUseCriticalSection class.  Now, no matter how you exit the function,
the critical section will automatically be "leaved."
----------------------------------------------------------------------*/

#ifndef _CRITICALSECTION_H
#define _CRITICALSECTION_H

/*//////////////////////////////////////////////////////////////////////
                       The CCriticalSection Class
//////////////////////////////////////////////////////////////////////*/
class CUseCriticalSection ;

class CCriticalSection
{
public      :

    CCriticalSection ( DWORD dwSpinCount = 4000 )
    {
        InitializeCriticalSectionAndSpinCount ( &m_CritSec  ,
                                                dwSpinCount  ) ;
    }
    ~CCriticalSection ( )
    {
        DeleteCriticalSection ( &m_CritSec ) ;
    }

    friend CUseCriticalSection ;
public     :
    CRITICAL_SECTION m_CritSec ;
} ;

/*//////////////////////////////////////////////////////////////////////
                     The CUseCriticalSection Class
//////////////////////////////////////////////////////////////////////*/
//lint -e1704
class CUseCriticalSection
{
public      :
    CUseCriticalSection ( const CCriticalSection & cs )
    {
        m_cs = &cs ;
        EnterCriticalSection ( ( LPCRITICAL_SECTION)&(m_cs->m_CritSec));
    }

    ~CUseCriticalSection ( )
    {
        LeaveCriticalSection ( (LPCRITICAL_SECTION)&(m_cs->m_CritSec) );
        m_cs = NULL ;
    }

private     :
    CUseCriticalSection ( void )
    {
        m_cs = NULL ;
    }
    const CCriticalSection * m_cs ;
} ;
//lint +e1704

#endif      // _CRITICALSECTION_H

