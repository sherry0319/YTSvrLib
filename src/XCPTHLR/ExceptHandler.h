/*  
Module : EXCEPTHANDLER.H
Purpose: Definition for a class which intercepts and logs details about unhandled exceptions.
Created: PJN / 14-03-2002
History: None

Copyright (c) 2002 - 2008 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////////// Macros / Defines  //////////////////////////////

#pragma once

#ifndef __EXCEPTHANDLER_H__
#define __EXCEPTHANDLER_H__

#define EXCEPTION_LOGGER_ENUMERATE_THREADS      0x00000001
#define EXCEPTION_LOGGER_ENUMERATE_MODULES      0x00000002
#define EXCEPTION_LOGGER_MODULE_SYMBOLS         0x00000004
#define EXCEPTION_LOGGER_REGISTERS              0x00000008
#define EXCEPTION_LOGGER_CALLSTACK              0x00000010
#define EXCEPTION_LOGGER_CALLSTACK_SYMBOLS      0x00000020
#define EXCEPTION_LOGGER_FILE_BUFFERING         0x00000040
#define EXCEPTION_LOGGER_RAW_CALLSTACK_CODE     0x00000100
#define EXCEPTION_LOGGER_RAW_CALLSTACK_STACK    0x00000200
#define EXCEPTION_LOGGER_ENUMERATE_PROCESSES    0x00000400
#define EXCEPTION_LOGGER_SYSTEM_DETAILS         0x00000800
#define EXCEPTION_LOGGER_ENVIRONMENT_STRINGS    0x00001000
#define EXCEPTION_LOGGER_DLL_VERSION_DETAILS    0x00002000
#define EXCEPTION_LOGGER_EXE_VERSION_DETAILS    0x00004000
#define EXCEPTION_LOGGER_MINIDUMP               0x00008000

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
////////////////////////////// Includes ///////////////////////////////////////

//#include <dbghelp.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <Wtsapi32.h>

//Taken from WinBase.h 
#ifndef HW_PROFILE_GUIDLEN
#define DOCKINFO_UNDOCKED          (0x1)
#define DOCKINFO_DOCKED            (0x2)
#define HW_PROFILE_GUIDLEN         39      // 36-characters plus NULL terminator
#define MAX_PROFILE_LEN            80
typedef struct tagHW_PROFILE_INFOA {
    DWORD  dwDockInfo;
    CHAR   szHwProfileGuid[HW_PROFILE_GUIDLEN];
    CHAR   szHwProfileName[MAX_PROFILE_LEN];
} HW_PROFILE_INFOA, *LPHW_PROFILE_INFOA;
typedef struct tagHW_PROFILE_INFOW {
    DWORD  dwDockInfo;
    WCHAR  szHwProfileGuid[HW_PROFILE_GUIDLEN];
    WCHAR  szHwProfileName[MAX_PROFILE_LEN];
} HW_PROFILE_INFOW, *LPHW_PROFILE_INFOW;
#ifdef UNICODE
typedef HW_PROFILE_INFOW HW_PROFILE_INFO;
typedef LPHW_PROFILE_INFOW LPHW_PROFILE_INFO;
#else
typedef HW_PROFILE_INFOA HW_PROFILE_INFO;
typedef LPHW_PROFILE_INFOA LPHW_PROFILE_INFO;
#endif // UNICODE

#endif


////////////////////////////// Classes //////////////////////////////

class CExceptionLogger
{
public:
//Constructors / Destructors
  CExceptionLogger();
  ~CExceptionLogger();

//Methods  
  static long WINAPI UnHandledExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo);
  static void  SetFlags(DWORD dwFlags) { m_dwFlags = dwFlags; };
  static DWORD GetFlags() { return m_dwFlags; };
  static void  SetDirectory(LPCTSTR pszDirectory);
  static LPCTSTR GetDirectory() { return m_szDirectory; };

protected:
//Member variables
  static TCHAR m_szLogFilename[_MAX_PATH];      //The path to the log file we will use
  static TCHAR m_szMiniDumpFilename[_MAX_PATH]; //The path to the mini dump file we will use
  static DWORD m_dwFlags;                       //Flags to control the logging details                 
  static TCHAR m_szDirectory[_MAX_PATH];        //The directory where the log and mini dump file are to be created

//Methods  
  static void  GenerateCrashLog(EXCEPTION_POINTERS* pExceptionInfo);
  static void  GenerateMiniDump(EXCEPTION_POINTERS* pExceptionInfo);
  static int   Log(const TCHAR* format, ...); 
  static BOOL  GetAddressDetails(void* pAddress, LPTSTR pszModule, DWORD& dwOffset, char* pszSectionName);
  static void  LogStack(CONTEXT* pContext, HANDLE hProcess);
  static BOOL  CALLBACK EnumSymbols(PSYMBOL_INFO pSymbolInfo, ULONG SymbolSize, PVOID pUserContext);
  static void  LogSymbol(PSYMBOL_INFO pSymbolInfo, STACKFRAME64* pStackFrame);
  static void  LogBasicSymbolDetails(DWORD dwBasicType, DWORD64 dwLength, void* pSymbolAddress);
  static BOOL  LogUDTSymbolDetails(DWORD dwTypeIndex, ULONG64 modBase, ULONG64 dwOffset, DWORD dwIndent);
  static DWORD GetBasicType(DWORD dwTypeIndex, DWORD64 modBase);
  static void  LogRawMemory(LPBYTE lpStart, DWORD dwPageSize);
  static void  LogProcessDetails(HANDLE hToolHelp32Snap);
  static void  LogModuleDetails(HANDLE hToolHelp32Snap, DWORD dwProcessID, BOOL bDisplayVariables, HANDLE hProcess);
  static void  LogThreadDetails(HANDLE hToolHelp32Snap, DWORD dwProcessID);
  static void  LogRegisters(EXCEPTION_POINTERS* pExceptionInfo);
  static void  LogSystemDetails();
  static void  LogEnvironmentStrings();
  static void  LogFileVersionDetails(LPCTSTR pszModule);
  static void  FormFileNames();
  void ResetToolHelpFunctionPointers();
  void ResetDbgHelpFunctionPointers();
  void ResetWTSFunctionPointers();
  void ResetPsapiFunctionPointers();
  void ResetAdvapi32FunctionPointers();

//Typedefs
  typedef HANDLE (WINAPI CREATETOOLHELP32SNAPSHOT)(DWORD, DWORD);
  typedef CREATETOOLHELP32SNAPSHOT* LPCREATETOOLHELP32SNAPSHOT;

  typedef BOOL (WINAPI THREAD32FIRST)(HANDLE, LPTHREADENTRY32);
  typedef THREAD32FIRST* LPTHREAD32FIRST;

  typedef BOOL (WINAPI THREAD32NEXT)(HANDLE, LPTHREADENTRY32);
  typedef THREAD32NEXT* LPTHREAD32NEXT;

  typedef BOOL (WINAPI MODULE32FIRST)(HANDLE, LPMODULEENTRY32);
  typedef MODULE32FIRST* LPMODULE32FIRST;

  typedef BOOL (WINAPI MODULE32NEXT)(HANDLE, LPMODULEENTRY32);
  typedef MODULE32NEXT* LPMODULE32NEXT;

  typedef HANDLE (WINAPI OPENTHREAD)(DWORD, BOOL, DWORD);
  typedef OPENTHREAD* LPOPENTHREAD;

  typedef BOOL (WINAPI SYMCLEANUP)(HANDLE);
  typedef SYMCLEANUP* LPSYMCLEANUP;

  typedef BOOL (WINAPI SYMENUMSYMBOLS)(HANDLE, ULONG64, PCSTR, PSYM_ENUMERATESYMBOLS_CALLBACK, PVOID);
  typedef SYMENUMSYMBOLS* LPSYMENUMSYMBOLS; 

  typedef DWORD (WINAPI SYMSETOPTIONS)(DWORD);
  typedef SYMSETOPTIONS* LPSYMSETOPTIONS;

  typedef BOOL (WINAPI SYMINITIALIZE)(HANDLE, PSTR, BOOL);
  typedef SYMINITIALIZE* LPSYMINITIALIZE;

  typedef ULONG (WINAPI SYMSETCONTEXT)(HANDLE, PIMAGEHLP_STACK_FRAME, PIMAGEHLP_CONTEXT);
  typedef SYMSETCONTEXT* LPSYMSETCONTEXT;

  typedef BOOL (WINAPI SYMGETLINEFROMADDR64)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
  typedef SYMGETLINEFROMADDR64* LPSYMGETLINEFROMADDR64;

  typedef BOOL (WINAPI SYMFROMADDR)(HANDLE, DWORD64, PDWORD64, PSYMBOL_INFO);
  typedef SYMFROMADDR* LPSYMFROMADDR;

  typedef BOOL (WINAPI STACKWALK64)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
  typedef STACKWALK64* LPSTACKWALK64;

  typedef PVOID (WINAPI SYMFUNCTIONTABLEACCESS64)(HANDLE, DWORD64);
  typedef SYMFUNCTIONTABLEACCESS64* LPSYMFUNCTIONTABLEACCESS64;

  typedef DWORD64 (WINAPI SYMGETMODULEBASE64)(HANDLE, DWORD64);
  typedef SYMGETMODULEBASE64* LPSYMGETMODULEBASE64;

  typedef BOOL (WINAPI SYMGETTYPEINFO)(HANDLE, DWORD64, ULONG, IMAGEHLP_SYMBOL_TYPE_INFO, PVOID);
  typedef SYMGETTYPEINFO* LPSYMGETTYPEINFO;

  typedef BOOL (WINAPI PROCESS32FIRST)(HANDLE, LPPROCESSENTRY32);
  typedef PROCESS32FIRST* LPPROCESS32FIRST;

  typedef BOOL (WINAPI PROCESS32NEXT)(HANDLE, LPPROCESSENTRY32);
  typedef PROCESS32NEXT* LPPROCESS32NEXT;

  typedef BOOL (WINAPI GLOBALMEMORYSTATUSEX)(LPMEMORYSTATUSEX);
  typedef GLOBALMEMORYSTATUSEX* LPGLOBALMEMORYSTATUSEX;

  typedef DWORD (WINAPI GETGUIRESOURCES)(HANDLE, DWORD);
  typedef GETGUIRESOURCES* LPGETGUIRESOURCES;

  typedef BOOL (WINAPI GETPROCESSMEMORYINFO)(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
  typedef GETPROCESSMEMORYINFO* LPGETPROCESSMEMORYINFO;

  typedef BOOL (WINAPI ISWOW64PROCESS)(HANDLE, PBOOL);
  typedef ISWOW64PROCESS* LPISWOW64PROCESS;

  typedef BOOL (WINAPI GETPROCESSHANDLECOUNT)(HANDLE, PDWORD);
  typedef GETPROCESSHANDLECOUNT* LPGETPROCESSHANDLECOUNT;

  typedef BOOL (WINAPI PROCESSIDTOSESSIONID)(DWORD, DWORD*);
  typedef PROCESSIDTOSESSIONID* LPPROCESSIDTOSESSIONID;

  typedef void (WINAPI GETNATIVESYSTEMINFO)(LPSYSTEM_INFO);
  typedef GETNATIVESYSTEMINFO* LPGETNATIVESYSTEMINFO;

  typedef UINT (WINAPI GETSYSTEMWOW64DIRECTORY)(LPTSTR, UINT);
  typedef GETSYSTEMWOW64DIRECTORY* LPGETSYSTEMWOW64DIRECTORY;

  typedef BOOL (WINAPI ISPROCESSORFEATUREPRESENT)(DWORD);
  typedef ISPROCESSORFEATUREPRESENT* LPISPROCESSORFEATUREPRESENT;

  typedef UINT (WINAPI GETSYSTEMWINDOWSDIRECTORY)(LPTSTR, UINT);
  typedef GETSYSTEMWINDOWSDIRECTORY* LPGETSYSTEMWINDOWSDIRECTORY;

  typedef DWORD (WINAPI WTSGETACTIVECONSOLESESSIONID)();
  typedef WTSGETACTIVECONSOLESESSIONID* LPWTSGETACTIVECONSOLESESSIONID;

  typedef BOOL (WINAPI WTSQUERYSESSIONINFORMATION)(HANDLE, DWORD, WTS_INFO_CLASS, LPTSTR*, DWORD*);
  typedef WTSQUERYSESSIONINFORMATION* LPWTSQUERYSESSIONINFORMATION;

  typedef void (WINAPI WTSFREEMEMORY)(PVOID);
  typedef WTSFREEMEMORY* LPWTSFREEMEMORY;

  typedef BOOL (WINAPI GETPERFORMANCEINFO)(PPERFORMANCE_INFORMATION, DWORD);
  typedef GETPERFORMANCEINFO* LPGETPERFORMANCEINFO;

  typedef BOOL (WINAPI GETCURRENTHWPROFILE)(LPHW_PROFILE_INFO);
  typedef GETCURRENTHWPROFILE* LPGETCURRENTHWPROFILE;
  
  typedef BOOL (WINAPI MINIDUMPWRITEDUMP)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);
  typedef MINIDUMPWRITEDUMP* LPMINIDUMPWRITEDUMP;

//Member variables
  static LPCREATETOOLHELP32SNAPSHOT     m_lpfnCreateToolhelp32Snapshot;
  static LPTHREAD32FIRST                m_lpfnThread32First;
  static LPTHREAD32NEXT                 m_lpfnThread32Next;
  static LPMODULE32FIRST                m_lpfnModule32First;
  static LPMODULE32NEXT                 m_lpfnModule32Next;
  static LPPROCESS32FIRST               m_lpfnProcess32First;
  static LPPROCESS32NEXT                m_lpfnProcess32Next;
  static LPOPENTHREAD                   m_lpfnOpenThread;
  static LPISWOW64PROCESS               m_lpfnIsWow64Process;
  static LPGLOBALMEMORYSTATUSEX         m_lpfnGlobalMemoryStatusEx;
  static LPSYMCLEANUP                   m_lpfnSymCleanup;
  static LPSYMENUMSYMBOLS               m_lpfnSymEnumSymbols;
  static LPSYMSETOPTIONS                m_lpfnSymSetOptions;
  static LPSYMINITIALIZE                m_lpfnSymInitialize;
  static LPSYMSETCONTEXT                m_lpfnSymSetContext;
  static LPSYMGETLINEFROMADDR64         m_lpfnSymGetLineFromAddr64;
  static LPSYMFROMADDR                  m_lpfnSymFromAddr;
  static LPSTACKWALK64                  m_lpfnStackWalk64;
  static LPSYMFUNCTIONTABLEACCESS64     m_lpfnSymFunctionTableAccess64;
  static LPSYMGETMODULEBASE64           m_lpfnSymGetModuleBase64;
  static LPSYMGETTYPEINFO               m_lpfnSymGetTypeInfo;
  static HMODULE                        m_hDbgHelp;
  static LPGETGUIRESOURCES              m_lpfnGetGuiResources;
  static HMODULE                        m_hPsapi;
  static LPGETPROCESSMEMORYINFO         m_lpfnGetProcessMemoryInfo;
  static LPGETPROCESSHANDLECOUNT        m_lpfnGetProcessHandleCount;
  static LPGETPERFORMANCEINFO           m_lpfnGetPerformanceInfo;
  static LPPROCESSIDTOSESSIONID         m_lpfnProcessIdToSessionId;
  static LPGETNATIVESYSTEMINFO          m_lpfnGetNativeSystemInfo;
  static LPGETSYSTEMWOW64DIRECTORY      m_lpfnGetSystemWow64Directory;
  static LPISPROCESSORFEATUREPRESENT    m_lpfnIsProcessorFeaturePresent;
  static LPGETSYSTEMWINDOWSDIRECTORY    m_lpfnGetSystemWindowsDirectory;
  static HMODULE                        m_hWtsApi32;
  static LPWTSGETACTIVECONSOLESESSIONID m_lpfnWTSGetActiveConsoleSessionId;
  static LPWTSQUERYSESSIONINFORMATION   m_lpfnWTSQuerySessionInformation;
  static LPWTSFREEMEMORY                m_lpfnWTSFreeMemory;
  static HMODULE                        m_hAdvapi32;
  static LPGETCURRENTHWPROFILE          m_lpfnGetCurrentHwProfile;
  static LPMINIDUMPWRITEDUMP            m_lpfnMiniDumpWriteDump;
  static LPTOP_LEVEL_EXCEPTION_FILTER   m_pPreviousFilter;                          //The previous exception filter which we chain to
  static HANDLE                         m_hLogFile;                                 //The open handle of the log file
  static TCHAR                          m_szTempLogLine[4096];                      //The static buffer we use for writing each log line
  static TCHAR                          m_szTempFileName[_MAX_PATH];                //The static buffer we use for any paths which need to be written out
  static BYTE                           m_bySymbolInfo[sizeof(SYMBOL_INFO) + 1024]; //The static buffer we use for storing symbol info in
};

#endif //__EXCEPTHANDLER_H__
