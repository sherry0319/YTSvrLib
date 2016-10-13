/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright ?1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _SUPERASSERT_H
#define _SUPERASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

// The macro end users will define in order to get the email address
// SUPERASSERT will use to direct the email.
// To override the email address do the following.  The email address
// must be in complete SMTP form, have quotes around the name, and
// be set to ANSI format.
// #ifdef SUPERASSERT_EMAIL
// #undef SUPERASSERT_EMAIL
// #define SUPERASSERT_EMAIL "john@wintellect.com"
// #endif

#ifndef SUPERASSERT_EMAIL
#define SUPERASSERT_EMAIL NULL
#endif

// Assertions are only active in _DEBUG builds.
#ifdef _DEBUG

/*----------------------------------------------------------------------
FUNCTION        :   SuperAssertion
DISCUSSION      :
    The assert function for C and C++ programs
PARAMETERS      :
    szType        - The type of assertion, "Assertion" or "Verify".
    szExpression  - The invalid expression.
    szFunction    - The function with the problem.
    szFile        - The file name where the assertion fired.
    iLine         - The line where the assertion fired.
    szEmail       - The email address to send the assertion to.  This
                    can be NULL and the email button will be disabled.
    dwStack       - The stack at the time of the assertion.
    dwStackFrame  - The stack frame at the time of the assertion.
    piFailCount   - The local fail count for the assertion.
    piIgnoreCount - The local ignore count for the assertion.
RETURNS         :
    FALSE - Ignore the assert.
    TRUE  - Trigger the DebugBreak.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL
    SuperAssertionA ( LPCSTR  szType        ,
                      LPCSTR  szExpression  ,
                      LPCSTR  szFunction    ,
                      LPCSTR  szFile        ,
                      int     iLine         ,
                      LPCSTR  szEmail       ,
                      DWORD64 dwStack       ,
                      DWORD64 dwStackFrame  ,
                      int *   piFailCount   ,
                      int *   piIgnoreCount  ) ;
                      
BUGSUTIL_DLLINTERFACE BOOL
    SuperAssertionW ( LPCWSTR szType        ,
                      LPCWSTR szExpression  ,
                      LPCWSTR szFunction    ,
                      LPCWSTR szFile        ,
                      int     iLine         ,
                      LPCSTR  szEmail       ,
                      DWORD64 dwStack       ,
                      DWORD64 dwStackFrame  ,
                      int *   piFailCount   ,
                      int *   piIgnoreCount  ) ;

#ifdef UNICODE
#define SuperAssertion SuperAssertionW
#else
#define SuperAssertion SuperAssertionA
#endif

#define NEWASSERT( exp ) NEWASSERT_REALMACRO( exp , "Assertion" )
// If you add local variables to this macro, make sure to update the
// g_szAssertLocalVars array in AssertDlg.CPP so they don't show up in
// the locals output and confuse the user.
#ifdef _M_IX86
#define NEWVERIFY( exp ) NEWASSERT_REALMACRO( exp , "Verify" )
#define NEWASSERT_REALMACRO( exp , type )                              \
{                                                                      \
    /* The local instance of the ignore count and the total hits. */   \
    static int sIgnoreCount = 0 ;                                      \
    static int sFailCount   = 0 ;                                      \
    /* The local stack and frame at the assertion's location. */       \
    DWORD dwStack ;                                                    \
    DWORD dwStackFrame ;                                               \
    /* Check the expression. */                                        \
    if ( ! ( exp ) )                                                   \
    {                                                                  \
        /* Houston, we have a problem. */                              \
        _asm { MOV dwStack , ESP }                                     \
        _asm { MOV dwStackFrame , EBP }                                \
        if ( TRUE == SuperAssertion ( TEXT ( type )         ,          \
                                      TEXT ( #exp )         ,          \
                                      TEXT ( __FUNCTION__ ) ,          \
                                      TEXT ( __FILE__ )     ,          \
                                      __LINE__              ,          \
                                      SUPERASSERT_EMAIL     ,          \
                                      (DWORD64)dwStack      ,          \
                                      (DWORD64)dwStackFrame ,          \
                                      &sFailCount           ,          \
                                      &sIgnoreCount          ) )       \
        {                                                              \
            __asm INT 3                                                \
        }                                                              \
    }                                                                  \
}
#else

#define NEWVERIFY( exp ) ((void)exp)
#define NEWASSERT_REALMACRO( exp , type ) 

#endif  // _M_IX86

/*----------------------------------------------------------------------
FUNCTION        :   GetSuperAssertionCount
DISCUSSION      :
    Returns the number of times assertions have fired inside the
SUPERASSERT system.  The returned value takes into account any form of
Ignoring assertions so it's always shows all assertions.
PARAMETERS      :
    None.
RETURNS         :
    The number of assertions.
----------------------------------------------------------------------*/
int BUGSUTIL_DLLINTERFACE GetSuperAssertionCount ( void ) ;

/*//////////////////////////////////////////////////////////////////////
                         _DEBUG IS NOT DEFINED!
//////////////////////////////////////////////////////////////////////*/
#else

#define NEWASSERT( exp )
#define NEWVERIFY( exp ) ((void)exp)

#endif  // #ifdef _DEBUG

#ifndef USE_OLD_ASSERTS
#define ASSERT NEWASSERT
#define VERIFY NEWVERIFY
#define SUPERASSERT NEWASSERT
#endif  //  USE_OLD_ASSERTS


#ifdef __cplusplus
}
#endif


#endif  // _SUPERASSERT_H


