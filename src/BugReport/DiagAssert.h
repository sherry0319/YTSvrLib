/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright ?1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/

#ifndef _DIAGASSERT_H
#define _DIAGASSERT_H

#ifdef __cplusplus
extern "C" {
#endif  //__cplusplus

/*//////////////////////////////////////////////////////////////////////
                                Defines
//////////////////////////////////////////////////////////////////////*/
// Keep the core stuff available in both release and debug builds.
// Uses the global assert flags.
#define DA_USEDEFAULTS      0x0000
// Turns on showing the assert in a messagebox.  This is the default.
#define DA_SHOWMSGBOX       0x0001
// Turns on showing the assert as through OutputDebugString.  This is
// the default.
#define DA_SHOWODS          0x0002
// Shows a stack trace in the assert.  This is off by default with the
// ASSERT macro and on in the SUPERASSERT macro.
#define DA_SHOWSTACKTRACE   0x0004
// Shows the output into the event log.  This is off by default for
// all macros.
#define DA_SHOWEVENTLOG     0x0008

/*----------------------------------------------------------------------
FUNCTION        :   SetDiagAssertOptions
DISCUSSION      :
    Sets the global options for normal ASSERT macros.
PARAMETERS      :
    dwOpts - The new options flags
RETURNS         :
    The previous options
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE DWORD __stdcall
    SetDiagAssertOptions ( DWORD dwOpts ) ;

/*----------------------------------------------------------------------
FUNCTION        :   SetDiagAssertFile
DISCUSSION      :
    Sets a HANDLE where the data in any assertion will be written.  To
turn off logging, call this function with INVALID_HANDLE_VALUE.  The
options set in SetDiagAssertOptions still apply; this function just
lets you log the assertion information to a file.
    No error checking is done on the file handle or on any writes to it.
PARAMETERS      :
    hFile - The file handle
RETURNS         :
    The previous file handle
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE HANDLE __stdcall
    SetDiagAssertFile ( HANDLE hFile ) ;

/*----------------------------------------------------------------------
FUNCTION        :   AddDiagAssertModule
DISCUSSION      :
    Adds the specified module to the list of modules that error strings
will be pulled from
PARAMETERS      :
    hMod - The module to add
RETURNS         :
    TRUE  - The module was added.
    FALSE - The internal table is full.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall
    AddDiagAssertModule ( HMODULE hMod ) ;

/*----------------------------------------------------------------------
FUNCTION        :   DiagAssert
DISCUSSION      :
    The assert function for C and C++ programs
PARAMETERS      :
    dwOverrideOpts - The DA_* options to override the global defaults
                     for this call into DiagAssert
    szMsg          - The message to show
    szFile         - The file that showed the assert
    dwLine         - The line that had the assert
RETURNS         :
    FALSE - Ignore the assert.
    TRUE  - Trigger the DebugBreak.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall
    DiagAssertA ( DWORD     dwOverrideOpts  ,
                  LPCSTR    szMsg           ,
                  LPCSTR    szFile          ,
                  DWORD     dwLine           ) ;

BUGSUTIL_DLLINTERFACE BOOL __stdcall
    DiagAssertW ( DWORD     dwOverrideOpts  ,
                  LPCWSTR   szMsg           ,
                  LPCWSTR   szFile          ,
                  DWORD     dwLine           ) ;

#ifdef UNICODE
#define DiagAssert  DiagAssertW
#else
#define DiagAssert  DiagAssertA
#endif

/*----------------------------------------------------------------------
FUNCTION        :   SetDiagOutputFile
DISCUSSION      :

    Sets a HANDLE where the data in any trace statements will optionally
be written.  To turn off logging, call this function with
INVALID_HANDLE_VALUE.
    No error checking is done on the file handle or on any writes to it.
PARAMETERS      :
    hFile - The file handle
RETURNS         :
    The previous file handle
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE HANDLE __stdcall
    SetDiagOutputFile ( HANDLE hFile ) ;

/*----------------------------------------------------------------------
FUNCTION        :   DiagOutput
DISCUSSION      :
    Provides a tracing routine to send strings through
OutputDebugString
PARAMETERS      :
    szFmt - The format string
    ...   - Parameters that will be expanded into szFmt
RETURNS         :
    None.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE void
    DiagOutputA ( LPCSTR szFmt , ... ) ;

BUGSUTIL_DLLINTERFACE void
    DiagOutputW ( LPCWSTR szFmt , ... ) ;

#ifdef UNICODE
#define DiagOutput  DiagOutputW
#else
#define DiagOutput  DiagOutputA
#endif

/*//////////////////////////////////////////////////////////////////////
                               UNDEFINES
//////////////////////////////////////////////////////////////////////*/
#ifdef ASSERT
#undef ASSERT
#endif

#ifdef assert
#undef assert
#endif

#ifdef VERIFY
#undef VERIFY
#endif
#ifdef TRACE
#undef TRACE
#endif

#ifdef TRACE0
#undef TRACE0
#endif
#ifdef TRACE1
#undef TRACE1
#endif
#ifdef TRACE2
#undef TRACE2
#endif
#ifdef TRACE3
#undef TRACE3
#endif


/*//////////////////////////////////////////////////////////////////////
                           _DEBUG Is Defined
//////////////////////////////////////////////////////////////////////*/
#ifdef _DEBUG

/*//////////////////////////////////////////////////////////////////////
                                Defines
//////////////////////////////////////////////////////////////////////*/
// The different global options that can be set with
// SetDiagAssertOptions. If any of these options are passed to DiagAssert in
// the first parameter, that value will override whatever the
// global settings are.

// The assert macro used by ASSERT and SUPERASSERT.
// Turn off "conditional expression is constant" because of while(0).
// I need to turn this off globally because the compilation error
// occurs on the expansion of the macro.
#pragma warning ( disable : 4127 )
#ifdef PORTABLE_BUGSLAYERUTIL
#define OLDASSERTMACRO(a,x)                                         \
    do                                                              \
    {                                                               \
        if ( !(x) &&                                                \
             DiagAssert ( a                 ,                       \
                          _T ( #x )         ,                       \
                          _T ( __FILE__  )  ,                       \
                          __LINE__)          )                      \
        {                                                           \
                DebugBreak ( ) ;                                    \
        }                                                           \
    } while (0)
#else   //!PORTABLE_BUGSLAYERUTIL
#define OLDASSERTMACRO(a,x)                                         \
    do                                                              \
    {                                                               \
        if ( !(x) &&                                                \
             DiagAssert ( a                 ,                       \
                          _T ( #x )         ,                       \
                          _T ( __FILE__  )  ,                       \
                          __LINE__)          )                      \
        {                                                           \
                __asm int 3                                         \
        }                                                           \
    } while (0)
#endif  // PORTABLE_BUGSLAYERUTIL

// The normal assert. It just uses the module defaults.
#define OLDASSERT(x) OLDASSERTMACRO(DA_USEDEFAULTS,x)

// Do the lowercase one.
#define assert OLDASSERT

// Trust, but verify.
#define OLDVERIFY(x)   OLDASSERT(x)

// Full-blown assert with all the trimmings
#define OLDSUPERASSERT(x) OLDASSERTMACRO(DA_SHOWSTACKTRACE |    \
                                            DA_SHOWMSGBOX  |    \
                                            DA_SHOWODS      ,   \
                                          x                  )

// The options macro
#define SETDIAGASSERTOPTIONS(x) SetDiagAssertOptions(x)

// The add module macro
#define ADDDIAGASSERTMODULE(x) AddDiagAssertModule(x)

// The TRACE macros
#ifdef __cplusplus
#define TRACE   ::DiagOutputA
#endif

#define TRACE0(sz)              DiagOutputA( "%s" , sz )
#define TRACE1(sz, p1)          DiagOutputA( sz , p1)
#define TRACE2(sz, p1, p2)      DiagOutputA( sz , p1, p2)
#define TRACE3(sz, p1, p2, p3)  DiagOutputA( sz , p1, p2, p3)

#else   // !_DEBUG
/*//////////////////////////////////////////////////////////////////////
                       _DEBUG Is !!NOT!! Defined
//////////////////////////////////////////////////////////////////////*/

#define OLDASSERTMACRO(a,x)
#define OLDASSERT(x)
#define OLDVERIFY(x)   ((void)(x))
#define OLDSUPERASSERT(x)
#define SETDIAGASSERTOPTIONS(x)
#define ADDDIAGASSERTMODULE(x)

#ifdef __cplusplus
//inline void TraceOutput(LPCTSTR, ...) { }
#if (_MSC_VER >= 1300)
#define TRACE   __noop
#else
#define TRACE   (void)0
#endif
#endif  // __cplusplus

#define TRACE0(fmt)
#define TRACE1(fmt,arg1)
#define TRACE2(fmt,arg1,arg2)
#define TRACE3(fmt,arg1,arg2,arg3)

#endif  // _DEBUG


#ifdef USE_OLD_ASSERTS
#define ASSERT OLDASSERT
#define VERIFY OLDVERIFY
#define SUPERASSERT OLDSUPERASSERT
#endif  //  USE_OLD_ASSERTS

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // _DIAGASSERT_H


