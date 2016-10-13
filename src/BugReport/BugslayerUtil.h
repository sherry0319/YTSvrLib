/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright ?1997-2003 John Robbins -- All rights reserved.

The BugslayerUtil.DLL file contains various routines that are useful
across various projects, not just those for a single column.  I will
be adding to the DLL in future columns.

HISTORY     :

FEB '98 - Initial version.  This also incorporated the October '97
          memory dumper and validator library.

APR '98 - Added the CSymbolEngine class in SymbolEngine.h

AUG '98 - Added the Crash Handler functions to assist in crash
          information.
        - Set all exported functions to __stdcall.

OCT '98 - Fixed HookImportedFunctionsByName to no patch addresses
          about 2GB on Win95.  Also changed the function so that
          the user can pass NULL for the returned orignal function
          array and NULL for the number of functions hooked.
        - Added the IsNT function to the export list.

FEB '99 - Updated the CrashHandler code to work on Win9x.
        - Updated CSymbolEngine to support getting the versions of
          IMAGEHLP.DLL as well as the PDB reader DLL, MSDBI.DLL or
          MSPDB50.DLL.
        - Moved to VC6.
        - Fixed bug reported by Frank McGrath in
          HookImportedFunctionsByName.
        - Added the ASSERT and SUPERASSERT cool macros all in
          DiagAssert.h/.cpp.
        - Removed the old ASSERT/TRACE stuff in MSJDBG.H.

JUN '99 - Fixed a bug where the TRACE macros did not link in UNICODE
          builds.
        - Fixed a bug reported by Dan Pierson in
          TLHELPGetLoadedModules.cpp where I was not setting the last
          error to the right value.
        - Added the HookOrdinalExport function.

DEC '99 - Many updates for the book.
        - CrashHandler.cpp updated for the latest and greatest
          DBGHELP.DLL.
        - SymbolEngine.h updated to use DBGHELP.DLL.

JUN '00 - Fixed a memory allocation mismatch bug in CrashHandler.cpp
          found by Walter Warniaha.
        - Fixed a bug in CrashHandler.cpp where I called FillMemory
          with a memory size of zero.  Found by Jim Miars.

NOV '00 - Added ultra cool new super assertion dialog!!

DEC '00 - Added the assertion output to the event log code.
        - Fixed two bugs in HookImportedFunctionsByName reported by
          Attila Szepesváry.
        - Fixed a bug reported by Craig Ball that Crash Handler was
          not returning a string for Microsoft C++ exception crashes.

JAN '01 - Added NEW_INLINE_WORKAROUND at the bottom of this file to
          fix Debug C Run Time memory tracking bug.

MAY '01 - Compiled code with early version of VC7 with /Wp64 switch
          and fixed 64-bit compiler warnings.
        - Updated all file copyrights.
        - Added event file output to the new super assert (I forgot!)

OCT '01 - Added MiniDump functionality.  This was not released as a
          column in MSDN Magazine, but in a newsletter from
          http://www.wintellect.com.

JAN '03 - Many, many changes for
          Debugging Applications for Microsoft .NET and Microsoft Windows 
            - The default is now UNICODE.
            - All TRACE statements use DiagOutputA so you don't have
              to do _T() macros around trace statements.
            - Added IsNT4, IsW2K, IsXP, IsServer2003, IsW2KorBetter, 
              IsXPorBetter, and IsServer2003orBetter.
            - Dropped all Win9x support.
            - Added the super, duper, majorly cool new assertion!
            - The BSUIsInteractiveUser function to determine if the
              process is running as a Win32 service or a logged in
              user.
            - ResString.h is now a first class header.
            - Added BSUSetThreadName.
            - Fixed BUGSUTIL_DLLINTERFACE usage.
            - Added BSU_SKIP_CRTDBG_INCLUDE.  Define this if you need
              to include this file before things like the ATL headers
              that include CRTDBG.H already.

----------------------------------------------------------------------*/

#ifndef _BUGSLAYERUTIL_H
#define _BUGSLAYERUTIL_H

/*//////////////////////////////////////////////////////////////////////
                                Includes
//////////////////////////////////////////////////////////////////////*/

// If windows.h has not yet been included, include it now.
#ifndef _INC_WINDOWS
#include <windows.h>
#endif

#include <tchar.h>

// Force imagehlp in as well as DBGHELP.H.
#include <imagehlp.h>
//#include <dbghelp.h>


/*//////////////////////////////////////////////////////////////////////
                            Special Defines
//////////////////////////////////////////////////////////////////////*/
// The defines that set up how the functions or classes are exported or
// imported.
#ifndef BUGSUTIL_DLLINTERFACE
#ifdef BUILDING_BUGSUTILITY_DLL
#define BUGSUTIL_DLLINTERFACE __declspec ( dllexport )
#else
#define BUGSUTIL_DLLINTERFACE __declspec ( dllimport )
#endif  // BUILDING_BUGSTILITY_DLL
#endif  // BUGSUTIL_DLLINTERFACE

/*//////////////////////////////////////////////////////////////////////
                            Special Includes
//////////////////////////////////////////////////////////////////////*/

// Include the header that sets up CRTDBG.H.
#ifndef BSU_SKIP_CRTDBG_INCLUDE
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

// Include the headers that do the memory dumping and validation
// routines.
#include "MemDumperValidator.h"

// The CrashHandler routines.
#include "CrashHandler.h"

// The old ASSERT routines.  These are the only ones that work for VB.
#include "DiagAssert.h"
// The new C/C++ routines.
#include "SuperAssert.h"

// The memory stress routines.
#include "MemStress.h"

// The minidump routines.
#include "MiniDump.h"

/*//////////////////////////////////////////////////////////////////////
                      C Function Declaration Area
                                 START
//////////////////////////////////////////////////////////////////////*/
#ifdef __cplusplus
extern "C" {
#endif  // _cplusplus

/*----------------------------------------------------------------------
FUNCTION        :   GetLoadedModules
DISCUSSION      :
    For the specified process id, this function returns the HMODULES for
all modules loaded into that process address space.  This function works
for both NT and 95.
PARAMETERS      :
    dwPID        - The process ID to look into.
    uiCount      - The number of slots in the paModArray buffer.  If
                   this value is 0, then the return value will be TRUE
                   and pdwRealCount will hold the number of items
                   needed.
    paModArray   - The array to place the HMODULES into.  If this buffer
                   is too small to hold the result and uiCount is not
                   zero, then FALSE is returned, but pdwRealCount will
                   be the real number of items needed.
    pdwRealCount - The count of items needed in paModArray, if uiCount
                   is zero, or the real number of items in paModArray.
RETURNS         :
    FALSE - There was a problem, check GetLastError.
    TRUE  - The function succeeded.  See the parameter discussion for
            the output parameters.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall
                           GetLoadedModules ( DWORD     dwPID        ,
                                              UINT      uiCount      ,
                                              HMODULE * paModArray   ,
                                              LPDWORD   pdwRealCount  );

/*----------------------------------------------------------------------
FUNCTION        :   GetProcessThreadIds
DISCUSSION      :
    For the specified process id, this function returns the thread IDs
only for threads in the specified process ID.  Unlike the toolhelp32
functions this function works like you would expect.
PARAMETERS      :
    dwPID        - The process ID to look into.
    uiCount      - The number of slots in the paThreadArray buffer.  If
                   this value is 0, then the return value will be TRUE
                   and pdwRealCount will hold the number of items
                   needed.
    paThreadArray- The array to place the thread IDs into.  If this
                   buffer is too small to hold the result and uiCount is
                   not zero, then FALSE is returned, but pdwRealCount
                   will be the real number of items needed.
    pdwRealCount - The count of items needed in paThreadArray, if
                   uiCount is zero, or the real number of items in
                   paModArray.
RETURNS         :
    FALSE - There was a problem, check GetLastError.
    TRUE  - The function succeeded.  See the parameter discussion for
            the output parameters.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall
                           GetProcessThreadIds ( DWORD   dwPID         ,
                                                 UINT    uiCount       ,
                                                 LPDWORD paThreadArray ,
                                                 LPDWORD pdwRealCount );

/*----------------------------------------------------------------------
FUNCTION        :   AllocAndFillProcessModuleList
DISCUSSION      :
    Hides the work to get the loaded module list for the current
process.  This function returns a pointer to memory allocated from the
specified heap and that memory must be deallocated with a call to
HeapFree from the same heap.
PARAMETERS      :
    hHeap    - The heap to allocate from.
    pdwCount - The total elements in the returned array.
RETURNS         :
    NULL  - There was a problem.
    !NULL - The block of memory holding the HMODULEs.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE HMODULE * __stdcall
                     AllocAndFillProcessModuleList ( HANDLE hHeap    ,
                                                     LPDWORD pdwCount );

typedef struct tag_HOOKFUNCDESC
{
    // The name of the function to hook.
    LPCSTR szFunc   ;
    // The procedure to blast in.
    PROC   pProc    ;
} HOOKFUNCDESC , * LPHOOKFUNCDESC ;

/*----------------------------------------------------------------------
FUNCTION        :   HookImportedFunctionsByName
DISCUSSION      :
    Hooks the specified functions imported into hModule by the module
indicated by szImportMod.  This function can be used to hook from one
to 'n' of the functions imported.  Note that this function is like
GetProcAddress in that the function names MUST be ANSI characters.
The imported module name can be ANSI or UNICODE.
    The techniques used in the function are slightly different than
that shown by Matt Pietrek in his book, "Windows 95 System Programming
Secrets."  He uses the address of the function to hook as returned by
GetProcAddress.  Unfortunately, while this works in almost all cases, it
does not work when the program being hooked is running under a debugger
on Windows95 (and presumably, Windows98).  The problem is that
GetProcAddress under a debugger returns a "debug thunk," not the address
that is stored in the Import Address Table (IAT).
    This function gets around that by using the real thunk list in the
PE file, the one not bashed by the loader when the module is loaded and
fixed up, to find where the named import is located.  Once the named
import is found, then the original table is blasted to make the hook.
As the name implies, this function will only hook functions imported by
name.
PARAMETERS      :
    hModule     - The module where the imports will be hooked.
    szImportMod - The name of the module whose functions will be
                  imported.
    uiCount     - The number of functions to hook.  This is the size of
                  the paHookArray and paOrigFuncs arrays.
    paHookArray - The array of function descriptors that list which
                  functions to hook.  At this point, the array does not
                  have to be in szFunc name order.  Also, if a
                  particular pProc is NULL, then that item will just be
                  skipped.  This makes it much easier for debugging.
    paOrigFuncs - The array of original addresses that were hooked.  If
                  a function was not hooked, then that item will be
                  NULL.  This parameter can be NULL if the returned
                  information is not needed.
    pdwHooked   - Returns the number of functions hooked out of
                  paHookArray.  This parameter can be NULL if the
                  returned information is not needed.
RETURNS         :
    FALSE - There was a problem, check GetLastError.
    TRUE  - The function succeeded.  See the parameter discussion for
            the output parameters.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall
        HookImportedFunctionsByNameA ( HMODULE         hModule     ,
                                       LPCSTR          szImportMod ,
                                       UINT            uiCount     ,
                                       LPHOOKFUNCDESC  paHookArray ,
                                       PROC *          paOrigFuncs ,
                                       LPDWORD         pdwHooked    ) ;

BUGSUTIL_DLLINTERFACE BOOL __stdcall
        HookImportedFunctionsByNameW ( HMODULE         hModule     ,
                                       LPCWSTR         szImportMod ,
                                       UINT            uiCount     ,
                                       LPHOOKFUNCDESC  paHookArray ,
                                       PROC *          paOrigFuncs ,
                                       LPDWORD         pdwHooked    ) ;

#ifdef UNICODE
#define HookImportedFunctionsByName HookImportedFunctionsByNameW
#else
#define HookImportedFunctionsByName HookImportedFunctionsByNameA
#endif

/*----------------------------------------------------------------------
FUNCTION        :   HookOrdinalExport
DISCUSSION      :
    Akin to HookImportedFunctionsByName, this function hooks functions
that are exported by ordinal.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                   THIS IS A VERY DANGEROUS FUNCTION
                 THIS FUNCTION IS NO LONGER SUPPORTED
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Only use this function is you are ABSOLUTELY sure that you know
exactly which export you are hooking.  If you hook the wrong one, you
can potentially cause serious damage.
PARAMETERS      :
    hModule     - The module where the imports will be hooked.
    szImportMod - The name of the module whose functions will be
                  imported.
    dwOrdinal   - The ordinal to hook.
    pHookFunc   - The function to patch in.
    ppOrigAddr  - The original address.  This can be NULL if you
                  do not care about getting the orginal address.
RETURNS         :
    FALSE - There was a problem, check GetLastError.
    TRUE  - The function succeeded.  See the parameter discussion for
            the output parameters.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall
                    HookOrdinalExportA ( HMODULE hModule     ,
                                         LPCSTR  szImportMod ,
                                         DWORD   dwOrdinal   ,
                                         PROC    pHookFunc   ,
                                         PROC *  ppOrigAddr   ) ;

BUGSUTIL_DLLINTERFACE BOOL __stdcall
                    HookOrdinalExportW ( HMODULE hModule     ,
                                         LPCWSTR szImportMod ,
                                         DWORD   dwOrdinal   ,
                                         PROC    pHookFunc   ,
                                         PROC *  ppOrigAddr   ) ;

#ifdef UNICODE
#define HookOrdinalExport HookOrdinalExportW
#else
#define HookOrdinalExport HookOrdinalExportA
#endif

/*----------------------------------------------------------------------
FUNCTION        :   BSUGetModuleBaseName
DISCUSSION      :
    Returns the base name of the specified module in a manner that is
portable between NT and Win95/98.
PARAMETERS      :
    hProcess   - The handle to the process.  In Win95/98 this is
                 ignored.
    hModule    - The module to look up.  If this is NULL, then the
                 module returned is the executable.
    lpBaseName - The buffer that recieves the base name.
    nSize      - The size of the buffer.
RETURNS         :
    !0 - The length of the string copied to the buffer.
    0  - The function failed.  To get extended error information,
         call GetLastError
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE DWORD __stdcall
      BSUGetModuleBaseNameW ( HANDLE hProcess   ,
                             HMODULE hModule    ,
                             LPWSTR  lpBaseName ,
                             DWORD   nSize       ) ;

BUGSUTIL_DLLINTERFACE DWORD __stdcall
      BSUGetModuleBaseNameA ( HANDLE hProcess   ,
                             HMODULE hModule    ,
                             LPSTR   lpBaseName ,
                             DWORD   nSize       ) ;

#ifdef UNICODE
#define BSUGetModuleBaseName BSUGetModuleBaseNameW
#else
#define BSUGetModuleBaseName BSUGetModuleBaseNameA
#endif

/*----------------------------------------------------------------------
FUNCTION        :   BSUGetModuleFileNameEx
DISCUSSION      :
    Returns the full path name of the specified module in a manner that is
portable between NT, Win2K, and Win9x.
PARAMETERS      :
    dwPID      - The process ID.
    hProcess   - The handle to the process.
    hModule    - The module to look up.
    szFilename - The buffer that recieves the name.
    nSize      - The size of the buffer.
RETURNS         :
    !0 - The length of the string copied to the buffer.
    0  - The function failed.  To get extended error information,
         call GetLastError
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE DWORD __stdcall
      BSUGetModuleFileNameExA ( DWORD     dwPID        ,
                                HANDLE    hProcess     ,
                                HMODULE   hModule      ,
                                LPSTR     szFilename   ,
                                DWORD     nSize         ) ;

BUGSUTIL_DLLINTERFACE DWORD __stdcall
      BSUGetModuleFileNameExW ( DWORD     dwPID        ,
                                HANDLE    hProcess     ,
                                HMODULE   hModule      ,
                                LPWSTR    szFilename   ,
                                DWORD     nSize         ) ;
#ifdef UNICODE
#define BSUGetModuleFileNameEx BSUGetModuleFileNameExW
#else
#define BSUGetModuleFileNameEx BSUGetModuleFileNameExA
#endif


/*----------------------------------------------------------------------
FUNCTION        :   IsNT
DISCUSSION      :
    Returns TRUE if the operating system is NT based.  This will return
TRUE for NT4, W2K, XP, and Server 2003.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - This is an NT based OS.
    FALSE - It's Win9x.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall IsNT ( void ) ;

/*----------------------------------------------------------------------
FUNCTION        :   IsNT4
DISCUSSION      :
    Returns TRUE if the operating system is Windows NT 4.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - Running on NT4.
    FALSE - It's something else.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall IsNT4 ( void ) ;

/*----------------------------------------------------------------------
FUNCTION        :   IsW2K
DISCUSSION      :
    Returns TRUE if the operating system is Windows 2000.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - This is Windows 2000.
    FALSE - It's something else.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall IsW2K ( void ) ;

/*----------------------------------------------------------------------
FUNCTION        :   IsXP
DISCUSSION      :
    Returns TRUE if the operating system is Windows XP.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - This is WindowsXP.
    FALSE - It's something else.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall IsXP ( void ) ;

/*----------------------------------------------------------------------
FUNCTION        :   IsServer2003
DISCUSSION      :
    Returns TRUE if the operating system is Windows Server 2003.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - This is Windows Server 2003.
    FALSE - It's something else.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall IsServer2003 ( void ) ;


/*----------------------------------------------------------------------
FUNCTION        :   IsW2KorBetter
DISCUSSION      :
    Returns TRUE if the operating system Windows 2000 or higher.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - This at least Windows 2000
    FALSE - It's something else.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall IsW2KorBetter ( void ) ;

/*----------------------------------------------------------------------
FUNCTION        :   IsXPorBetter
DISCUSSION      :
    Returns TRUE if the operating system Windows XP or higher.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - This at least Windows XP
    FALSE - It's something else.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall IsXPorBetter ( void ) ;

/*----------------------------------------------------------------------
FUNCTION        :   IsServer2003orBetter
DISCUSSION      :
    Returns TRUE if the operating system Windows Server 2003 or higher.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - This at least Windows Server 2003
    FALSE - It's something else.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall IsServer2003orBetter ( void ) ;


/*----------------------------------------------------------------------
FUNCTION        :   BSUSymInitialize
DISCUSSION      :
    Under Win9x, you cannot set the SymInitialize fInvadeProcess flag to
TRUE.  This function will let you invade the process on all operating
systems when you initialize the symbol engine.

PARAMETERS      :
    dwPID          - The process ID for the process which all modules
                     are to be loaded.
    hProcess       - The normal SymInitialize parameter.
    UserSearchPath - The normal SymInitialize parameter.
    fInvadeProcess - The normal SymInitialize parameter.
RETURNS         :
    Whatever SymInitialize returns.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE DWORD __stdcall
        BSUSymInitializeA ( DWORD  dwPID          ,
                            HANDLE hProcess       ,
                            char * UserSearchPath ,
                            BOOL   fInvadeProcess  ) ;

BUGSUTIL_DLLINTERFACE DWORD __stdcall
        BSUSymInitializeW ( DWORD     dwPID          ,
                            HANDLE    hProcess       ,
                            wchar_t * UserSearchPath ,
                            BOOL      fInvadeProcess  ) ;

#ifdef UNICODE
#define BSUSymInitialize BSUSymInitializeW
#else
#define BSUSymInitialize BSUSymInitializeA
#endif  // UNICODE


/*----------------------------------------------------------------------
FUNCTION        :   BSUWide2Ansi
DISCUSSION      :
    A nice wrapper around WideCharToMultiByte to take care of most of
the normal parameters.  Uses the current thread ANSI code page and
calculates the length of the wide string.
PARAMETERS      :
    szWide   - The wide characters to convert.
    szANSI   - The ansi buffer to hold the conversion.
    iANSILen - The length in bytes of the szANSI buffer.
RETURNS         :
    TRUE  - The conversion went fine.
    FALSE - There was a problem.  Call GetLastError to find out.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE DWORD __stdcall
                               BSUWide2Ansi ( const wchar_t * szWide  ,
                                              char *          szANSI  ,
                                              int             iANSILen);

/*----------------------------------------------------------------------
FUNCTION        :   BSUAnsi2Wide
DISCUSSION      :
    A nice wrapper around MultiByteToWideChar to take care of most of
the normal parameters.  Uses the current thread ANSI code page and
calculates the length of the ANSI string.
PARAMETERS      :
    szANSI   - The ANSI characters to convert.
    szWide   - The UNICODE buffer to hold the conversion.
    iWideLen - The length in bytes of the szANSI buffer.
RETURNS         :
    TRUE  - The conversion went fine.
    FALSE - There was a problem.  Call GetLastError to find out.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE DWORD __stdcall
                                BSUAnsi2Wide ( const char * szANSI   ,
                                               wchar_t *    szWide   ,
                                               int          iWideLen  );


/*----------------------------------------------------------------------
FUNCTION        :   BSUIsInteractiveUser
DISCUSSION      :
    Returns TRUE if the current process is running in an interactive
user account.
PARAMETERS      :
    None.
RETURNS         :
    TRUE  - Running in an interactive logged in user process.
    FALSE - Running in a Win32 Service.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall BSUIsInteractiveUser ( void ) ;

/*----------------------------------------------------------------------
FUNCTION        :   BSUSetThreadName
DISCUSSION      :
    Sets the name of a native thread so it can be seen in the debugger.
I lifted this function from
ms-help://MS.MSDNQTR.2002JAN.1033/vsdebug/html/vxtskSettingThreadName.htm.
All I did was wrap it so it could be called from both UNICODE and ANSI
builds.
PARAMETERS      :
    dwThreadID   - The thread who's name is to be set.
    szThreadName - The name to assign the thread.
RETURNS         :
    None.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE void __stdcall
                            BSUSetThreadNameW ( DWORD   dwThreadID   ,
                                                LPCWSTR szThreadName  ) ;

BUGSUTIL_DLLINTERFACE void __stdcall
                            BSUSetThreadNameA ( DWORD   dwThreadID   ,
                                                LPCSTR  szThreadName  ) ;

#ifdef UNICODE
#define BSUSetThreadName BSUSetThreadNameW
#else
#define BSUSetThreadName BSUSetThreadNameA
#endif  // UNICODE

/*----------------------------------------------------------------------
FUNCTION        :   BSUSetCurrentThreadName
DISCUSSION      :
    Sets the name of the current thread so it can be seen in the
debugger.  I lifted this function from
ms-help://MS.MSDNQTR.2002JAN.1033/vsdebug/html/vxtskSettingThreadName.htm.
All I did was wrap it so it could be called from both UNICODE and ANSI
builds.
PARAMETERS      :
    dwThreadID   - The thread who's name is to be set.
    szThreadName - The name to assign the thread.
RETURNS         :
    None.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE void __stdcall
                    BSUSetCurrentThreadNameW ( LPCWSTR szThreadName  ) ;

BUGSUTIL_DLLINTERFACE void __stdcall
                    BSUSetCurrentThreadNameA ( LPCSTR szThreadName  ) ;

#ifdef UNICODE
#define BSUSetCurrentThreadName BSUSetCurrentThreadNameW
#else
#define BSUSetCurrentThreadName BSUSetCurrentThreadNameA
#endif  // UNICODE


#ifdef __cplusplus
}
#endif  // _cplusplus
/*//////////////////////////////////////////////////////////////////////
                                  END
                      C Function Declaration Area
//////////////////////////////////////////////////////////////////////*/

/*//////////////////////////////////////////////////////////////////////
                         C++ Only Declarations
                                 START
//////////////////////////////////////////////////////////////////////*/
#ifdef __cplusplus

#include "SymbolEngine.h"

// The quick string resources header.
#include "ResString.h"

// Fix the Debug C Run Time to work properly only if MFC is not
// defined.
#ifdef _MFC_VER
#define SKIPWORKAROUND 1
#endif

// #ifndef SKIPWORKAROUND
// #ifdef _DEBUG
// #ifndef NEW_INLINE_WORKAROUND
// #define NEW_INLINE_WORKAROUND new ( _NORMAL_BLOCK ,\
//                                     __FILE__ , __LINE__ )
// #define new NEW_INLINE_WORKAROUND
// #endif
// #endif  // _DEBUG
// #endif  // SKIPWORKAROUND


#endif  // __cplusplus
/*//////////////////////////////////////////////////////////////////////
                                  END
                         C++ Only Declarations
//////////////////////////////////////////////////////////////////////*/

#endif  // _BUGSLAYERUTIL_H