/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright ?1997-2003 John Robbins -- All rights reserved.

The minidump function wrappers.  These simply make it easier to call the
minidump function if the export doesn't happen to be there.

CreateCurrentProcessCrashDump is what you'll call in your exception
handlers to get a crash dump written.

SnapCurrentProcessMiniDump is for writing out dumps at any point during
the execution that are readable by the debuggers.  One big problem with
MiniDumpWriteDump is that for non-crash writes, is that it starts the
stack of the current thread way down deep inside MiniDumpWriteDump.
WinDBG is not able to walk any stacks other than those created with a
valid EXCEPTION_POINTERS.  VS.NET obviously uses a different stack
walking algorithm as it can walk the stack most of the time.  Since I
think snapping a mini dump is such a great debugging technique, I had to
fix it.

Note that these functions do NO checking if the older broken versions of
the mini dump commands are there.  The broken versions would hang if
called inside the process space.  Any version of DBGHELP.DLL from WinDBG
3.0.20 or later, or Windows XP will work just fine.

----------------------------------------------------------------------*/
#ifndef _MINIDUMP_H
#define _MINIDUMP_H

#ifdef __cplusplus
extern "C" {
#endif

/*//////////////////////////////////////////////////////////////////////
                         Typedefs and Structures
//////////////////////////////////////////////////////////////////////*/
// The return values for CreateCurrentProcessCrashDump and
// SnapCurrentProcessMiniDump.
typedef enum tag_BSUMDRET
{
    // Everything worked.
    eDUMP_SUCCEEDED           ,
    // DBGHELP.DLL could not be found at all in the path.
    eDBGHELP_NOT_FOUND        ,
    // The mini dump exports are not in the version of DBGHELP.DLL
    // in memory.
    eDBGHELP_MISSING_EXPORTS  ,
    // A parameter was bad.
    eBAD_PARAM                ,
    // Unable to open the dump file requested.
    eOPEN_DUMP_FAILED         ,
    // MiniDumpWriteDump failed.  Call GetLastError to see why.
    eMINIDUMPWRITEDUMP_FAILED ,
    // Unable to get the thread context.
    eGETTHREADCONTEXT_FAILED  ,
    // Unable to read the process memory.
    eREADPROCMEM_FAILED       ,
    // The invalid error value.
    eINVALID_ERROR            ,
} BSUMDRET ;

// This is the MINIDUMP_TYPE from the Windows XP.  If MINIDUMP_SIGNATURE
// is not defined, I'll use this one.  Otherwise, I'll get the one
// from the previously included DBGHELP.DLL.
#ifndef MINIDUMP_SIGNATURE
typedef enum _MINIDUMP_TYPE {
    MiniDumpNormal         = 0x0000,
    MiniDumpWithDataSegs   = 0x0001,
    MiniDumpWithFullMemory = 0x0002,
    MiniDumpWithHandleData = 0x0004,
} MINIDUMP_TYPE;
#endif  // MINIDUMP_SIGNATURE

/*----------------------------------------------------------------------
FUNCTION        :   IsMiniDumpFunctionAvailable
DISCUSSION      :
    Returns TRUE if the DBGHELP mini dump commands are available.  You
can use this as a quick check to see if the commands are there.
PARAMETERS      :
    None.
RETURNS         :
    FALSE - Mini dump functions are not available.
    TRUE  - Mini dump functions are there.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BOOL __stdcall
                                  IsMiniDumpFunctionAvailable ( void ) ;

/*----------------------------------------------------------------------
FUNCTION        :   CreateCurrentProcessCrashDump
DISCUSSION      :
    Creates a minidump of the current process.  Use this function to
write dumps from your exception handlers.
PARAMETERS      :
    eType       - The type of mini dump to do.
    szFileName  - The complete path and filename to write the dump.
                  Traditionally, the extension for dump files is .DMP.
                  If the file exists, it will be overwritten.
    dwThread    - The id of the thread that crashed.
    pExceptInfo - The exception information.
RETURNS         :
    See the BSUMDRET enum above.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BSUMDRET __stdcall
    CreateCurrentProcessCrashDumpA ( MINIDUMP_TYPE        eType      ,
                                     char *               szFileName ,
                                     DWORD                dwThread   ,
                                     EXCEPTION_POINTERS * pExceptInfo );

BUGSUTIL_DLLINTERFACE BSUMDRET __stdcall
    CreateCurrentProcessCrashDumpW ( MINIDUMP_TYPE        eType      ,
                                     wchar_t *            szFileName ,
                                     DWORD                dwThread   ,
                                     EXCEPTION_POINTERS * pExceptInfo );

#ifdef UNICODE
#define CreateCurrentProcessCrashDump CreateCurrentProcessCrashDumpW
#else
#define CreateCurrentProcessMiniDump CreateCurrentProcessCrashDumpA
#endif

/*----------------------------------------------------------------------
FUNCTION        :   SnapCurrentProcessMiniDump
DISCUSSION      :
    Creates a minidump of the current process that will properly open
and display the callstack in both WinDBG and VS.NET.  Use this to write
non-crash dumps (snaps) at any time.
PARAMETERS      :
    eType       - The type of mini dump to do.
    szFileName  - The complete path and filename to write the dump.
                  Traditionally, the extension for dump files is .DMP.
                  If the file exists, it will be overwritten.
    dwThread    - The optional id of the thread that crashed.
    pExceptInfo - The optional exception information.  This can be NULL
                  to indicate no exception information is to be added
                  to the dump.
RETURNS         :
    See the BSUMDRET enum above.
----------------------------------------------------------------------*/
BUGSUTIL_DLLINTERFACE BSUMDRET
            SnapCurrentProcessMiniDumpA ( MINIDUMP_TYPE eType      ,
                                          const char *  szDumpName  ) ;
BUGSUTIL_DLLINTERFACE BSUMDRET
            SnapCurrentProcessMiniDumpW ( MINIDUMP_TYPE   eType      ,
                                          const wchar_t * szDumpName  );


#ifdef UNICODE
#define SnapCurrentProcessMiniDump SnapCurrentProcessMiniDumpW
#else
#define SnapCurrentProcessMiniDump SnapCurrentProcessMiniDumpA
#endif

#ifdef __cplusplus
}
#endif

#endif // _MINIDUMP_H
