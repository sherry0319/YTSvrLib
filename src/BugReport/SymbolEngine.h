/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright ?1997-2003 John Robbins -- All rights reserved.
------------------------------------------------------------------------
Compilation Defines:

USE_BUGSLAYERUTIL - If defined, the class will have another
                    initialization method, BSUSymInitialize, which will
                    use BSUSymInitialize from BUGSLAYERUTIL.DLL to
                    initialize the symbol engine and allow the invade
                    process flag to work for all Win32 operating systems.
                    If you use this define, you must use
                    BUGSLAYERUTIL.H to include this file.
----------------------------------------------------------------------*/

#ifndef _SYMBOLENGINE_H
#define _SYMBOLENGINE_H

// You could include either IMAGEHLP.DLL or DBGHELP.DLL.
#include "imagehlp.h"
#include <tchar.h>

// Include the VariableInfo header.
#include "VariableInfo.h"

// The CodeView constants.
#include "CVConst.h"

// Include these in case the user forgets to link against them.
#pragma comment (lib,"dbghelp.lib")
#pragma comment (lib,"version.lib")

// The great Bugslayer idea of creating wrapper classes on structures
// that have size fields came from fellow MSJ columnist, Paul DiLascia.
// Thanks, Paul!

// I didn’t wrap IMAGEHLP_SYMBOL because that is a variable-size
// structure.

// The IMAGEHLP_MODULE wrapper class
struct CImageHlp_Module : public IMAGEHLP_MODULE64
{
    CImageHlp_Module ( )
    {
        memset ( this , 0 , sizeof ( IMAGEHLP_MODULE64 ) ) ;
        SizeOfStruct = sizeof ( IMAGEHLP_MODULE64 ) ;
    }
} ;

// The IMAGEHLP_LINE wrapper class
struct CImageHlp_Line : public IMAGEHLP_LINE64
{
    CImageHlp_Line ( )
    {
        memset ( this , 0 , sizeof ( IMAGEHLP_LINE64 ) ) ;
        SizeOfStruct = sizeof ( IMAGEHLP_LINE64 ) ;
    }
} ;

/*//////////////////////////////////////////////////////////////////////
// Typedefs for functions I have to wrap to ensure this code runs on
// the stock W2K DBGHELP.DLL
//////////////////////////////////////////////////////////////////////*/
typedef BOOL (WINAPI *PSYMENUMSYMBOLS) ( HANDLE                        ,
                                         ULONG64                       ,
                                         PCSTR                         ,
                                         PSYM_ENUMERATESYMBOLS_CALLBACK,
                                         PVOID                        );
typedef ULONG (WINAPI *PSYMSETCONTEXT) ( HANDLE                ,
                                         PIMAGEHLP_STACK_FRAME ,
                                         PIMAGEHLP_CONTEXT      ) ;
typedef BOOL (WINAPI *PSYMFROMADDR) ( HANDLE         ,
                                      DWORD64        ,
                                      PDWORD64       ,
                                      PSYMBOL_INFO    ) ;
typedef BOOL (WINAPI *PSYMFROMNAME) ( HANDLE         ,
                                      LPSTR          ,
                                      PSYMBOL_INFO    ) ;
typedef BOOL (WINAPI *PSYMGETTYPEFROMNAME) ( HANDLE            ,
                                             ULONG64           ,
                                             LPSTR             ,
                                             PSYMBOL_INFO       ) ;
typedef BOOL (WINAPI *PSYMGETTYPEINFO) ( HANDLE                     ,
                                         DWORD64                    ,
                                         ULONG                      ,
                                         IMAGEHLP_SYMBOL_TYPE_INFO  ,
                                         PVOID                       ) ;
typedef BOOL (WINAPI *PSYMENUMTYPES) (HANDLE                          ,
                                      ULONG64                         ,
                                      PSYM_ENUMERATESYMBOLS_CALLBACK  ,
                                      PVOID                           );

// The callback for enumerating local variables.
typedef BOOL (CALLBACK *PENUM_LOCAL_VARS_CALLBACK)
                                    ( DWORD64  dwAddr        ,
                                      LPCTSTR  szType        ,
                                      LPCTSTR  szName        ,
                                      LPCTSTR  szValue       ,
                                      int      iIndentLevel  ,
                                      PVOID    pContext       ) ;
                                      
// The UNICODE wrapper on UnDecorateSymbolName.
//DWORD UnDecorateSymbolNameW ( PCWSTR DecoratedName       ,
//                              PWSTR  UnDecoratedName     ,
//                              DWORD  UndecoratedLength   ,
//                              DWORD  Flags                ) ;

                                      

// The symbol engine class
class CSymbolEngine
{
/*----------------------------------------------------------------------
                   Public Construction and Destruction
----------------------------------------------------------------------*/
public      :
    // To use this class, call the SymInitialize member function to
    // initialize the symbol engine and then use the other member
    // functions in place of their corresponding DBGHELP.DLL functions.
    CSymbolEngine ( void )
    {
        m_hProcess = NULL ;
        // Default to hex local display.
        m_iRadix = 16 ;
        ZeroLocalEnumerationMemberVars ( ) ;
        
        m_pSymEnumSymbols = NULL ;
        m_pSymSetContext = NULL ;
        m_pSymFromAddr = NULL ;
        m_pSymFromName = NULL ;
        m_pSymGetTypeFromName = NULL ;
        m_pSymGetTypeInfo = NULL ;
        m_pSymEnumTypes = NULL ;
    }

    virtual ~CSymbolEngine ( void )
    {
    }

/*----------------------------------------------------------------------
                   Public Helper Information Functions
----------------------------------------------------------------------*/
public      :

    // Returns the file version of DBGHELP.DLL being used.
    //  To convert the return values into a readable format:
    //  wsprintf ( szVer                  ,
    //             _T ( "%d.%02d.%d.%d" ) ,
    //             HIWORD ( dwMS )        ,
    //             LOWORD ( dwMS )        ,
    //             HIWORD ( dwLS )        ,
    //             LOWORD ( dwLS )         ) ;
    //  szVer will contain a string like: 5.00.1878.1
    BOOL GetImageHlpVersion ( DWORD & dwMS , DWORD & dwLS )
    {
        return( GetInMemoryFileVersion ( _T ( "DBGHELP.DLL" ) ,
                                         dwMS                 ,
                                         dwLS                  ) ) ;
    }

    BOOL GetDbgHelpVersion ( DWORD & dwMS , DWORD & dwLS )
    {
        return( GetInMemoryFileVersion ( _T ( "DBGHELP.DLL" ) ,
                                         dwMS                 ,
                                         dwLS                  ) ) ;
    }

    // The worker function used by the previous two functions
    BOOL GetInMemoryFileVersion ( TCHAR * szFile ,
                                  DWORD & dwMS   ,
                                  DWORD & dwLS    ) ;
                                  
/*----------------------------------------------------------------------
                    Public Initialization and Cleanup
----------------------------------------------------------------------*/
public      :

    BOOL SymInitialize ( IN HANDLE   hProcess       ,
                         IN LPSTR    UserSearchPath ,
                         IN BOOL     fInvadeProcess  )
    {
        m_hProcess = hProcess ;
        return ( ::SymInitialize ( hProcess       ,
                                   UserSearchPath ,
                                   fInvadeProcess  ) ) ;
    }

#ifdef USE_BUGSLAYERUTIL
    BOOL BSUSymInitialize ( DWORD  dwPID          ,
                            HANDLE hProcess       ,
                            PSTR   UserSearchPath ,
                            BOOL   fInvadeProcess  )
    {
        m_hProcess = hProcess ;
        return ( ::BSUSymInitializeA ( dwPID          ,
                                       hProcess       ,
                                       UserSearchPath ,
                                       fInvadeProcess  ) ) ;
    }
#endif  // USE_BUGSLAYERUTIL
    BOOL SymCleanup ( void )
    {
        return ( ::SymCleanup ( m_hProcess ) ) ;
    }

/*----------------------------------------------------------------------
                        Public Module Manipulation
----------------------------------------------------------------------*/
public      :

    BOOL SymEnumerateModules64 ( IN PSYM_ENUMMODULES_CALLBACK64
                                                   EnumModulesCallback,
                                 IN PVOID             UserContext )
    {
        return ( ::SymEnumerateModules64 ( m_hProcess           ,
                                           EnumModulesCallback  ,
                                           UserContext           ) ) ;
    }
#ifndef _WIN64
    BOOL SymEnumerateModules ( IN PSYM_ENUMMODULES_CALLBACK64
                                                 EnumModulesCallback,
                               IN PVOID             UserContext )
    {
        return ( ::SymEnumerateModules64 ( m_hProcess           ,
                                           EnumModulesCallback  ,
                                           UserContext           ) ) ;
    }
#endif
    DWORD64 SymLoadModule64 ( IN  HANDLE     hFile       ,
                              IN  PSTR       ImageName   ,
                              IN  PSTR       ModuleName  ,
                              IN  DWORD64    BaseOfDll   ,
                              IN  DWORD      SizeOfDll    )
    {
        return ( ::SymLoadModule64 ( m_hProcess   ,
                                     hFile        ,
                                     ImageName    ,
                                     ModuleName   ,
                                     BaseOfDll    ,
                                     SizeOfDll     ) ) ;
    }

    DWORD64 SymLoadModule64 ( IN  HANDLE     hFile       ,
                              IN  PWSTR      ImageName   ,
                              IN  PWSTR      ModuleName  ,
                              IN  DWORD64    BaseOfDll   ,
                              IN  DWORD      SizeOfDll    ) ;


#ifndef _WIN64
    DWORD64 SymLoadModule ( IN  HANDLE     hFile       ,
                            IN  PSTR       ImageName   ,
                            IN  PSTR       ModuleName  ,
                            IN  DWORD64    BaseOfDll   ,
                            IN  DWORD      SizeOfDll    )
    {
        return ( ::SymLoadModule64 ( m_hProcess   ,
                                     hFile        ,
                                     ImageName    ,
                                     ModuleName   ,
                                     BaseOfDll    ,
                                     SizeOfDll     ) ) ;
    }
#endif

    DWORD64 SymLoadModuleEx ( IN  HANDLE         hProcess   ,
                              IN  HANDLE         hFile      ,
                              IN  PSTR           ImageName  ,
                              IN  PSTR           ModuleName ,
                              IN  DWORD64        BaseOfDll  ,
                              IN  DWORD          DllSize    ,
                              IN  PMODLOAD_DATA  Data       ,
                              IN  DWORD          Flags       )
    {
        return ( ::SymLoadModuleEx ( hProcess   ,
                                     hFile      ,
                                     ImageName  ,
                                     ModuleName ,
                                     BaseOfDll  ,
                                     DllSize    ,
                                     Data       ,
                                     Flags       ) ) ;
    }

    BOOL EnumerateLoadedModules64 ( IN PENUMLOADED_MODULES_CALLBACK64
                                              EnumLoadedModulesCallback,
                                    IN PVOID         UserContext       )
    {
        return ( ::EnumerateLoadedModules64 ( m_hProcess                ,
                                              EnumLoadedModulesCallback ,
                                              UserContext              ));
    }

#ifndef _WIN64
    BOOL EnumerateLoadedModules ( IN PENUMLOADED_MODULES_CALLBACK64
                                            EnumLoadedModulesCallback,
                                  IN PVOID         UserContext       )
    {
        return ( ::EnumerateLoadedModules64 ( m_hProcess                ,
                                              EnumLoadedModulesCallback ,
                                              UserContext              ));
    }
#endif

    BOOL SymUnloadModule64 ( IN  DWORD64 BaseOfDll )
    {
        return ( ::SymUnloadModule64 ( m_hProcess , BaseOfDll ) ) ;
    }

#ifndef _WIN64
   BOOL SymUnloadModule ( IN  DWORD64 BaseOfDll )
    {
        return ( ::SymUnloadModule64 ( m_hProcess , BaseOfDll ) ) ;
    }
#endif

    BOOL SymGetModuleInfo64 ( IN  DWORD64            dwAddr     ,
                              OUT PIMAGEHLP_MODULE64 ModuleInfo  )
    {
        return ( ::SymGetModuleInfo64 ( m_hProcess    ,
                                       dwAddr        ,
                                       ModuleInfo     ) ) ;
    }
    
    BOOL SymGetModuleInfoW64 ( IN  DWORD64                 qwAddr     ,
                               OUT PIMAGEHLP_MODULEW64     ModuleInfo  )
    {
        return ( ::SymGetModuleInfoW64 ( m_hProcess   ,
                                         qwAddr       ,
                                         ModuleInfo    ) ) ;
    }
    
#ifndef _WIN64
   BOOL SymGetModuleInfo ( IN  DWORD64            dwAddr     ,
                            OUT PIMAGEHLP_MODULE64 ModuleInfo  )
    {
        return ( ::SymGetModuleInfo64 ( m_hProcess    ,
                                       dwAddr        ,
                                       ModuleInfo     ) ) ;
    }
#endif

    DWORD64 SymGetModuleBase64 ( IN DWORD64 dwAddr )
    {
        return ( ::SymGetModuleBase64 ( m_hProcess , dwAddr ) ) ;
    }

#ifndef _WIN64
    DWORD64 SymGetModuleBase ( IN DWORD64 dwAddr )
    {
        return ( ::SymGetModuleBase64 ( m_hProcess , dwAddr ) ) ;
    }
 #endif

/*----------------------------------------------------------------------
                   Public Symbol and Type Manipulation
----------------------------------------------------------------------*/
    ULONG SymSetContext ( PIMAGEHLP_STACK_FRAME StackFrame  ,
                          PIMAGEHLP_CONTEXT     Context      ) ;
    
    BOOL SymEnumSymbols ( ULONG64   BaseOfDll               ,
                          PCSTR     Mask                    ,
                          PSYM_ENUMERATESYMBOLS_CALLBACK
                                    EnumSymbolsCallback     ,
                          PVOID     UserContext              ) ;
    
    BOOL SymFromAddr ( DWORD64       Address        ,
                       PDWORD64      Displacement   ,
                       PSYMBOL_INFO  Symbol          ) ;
    
    BOOL SymFromName ( LPSTR        Name    ,
                       PSYMBOL_INFO Symbol   ) ;

    BOOL SymGetTypeFromName ( ULONG64       BaseOfDll ,
                              LPSTR         Name      ,
                              PSYMBOL_INFO  Symbol     ) ;
    
    BOOL SymGetTypeInfo ( DWORD64                    ModBase ,
                          ULONG                      TypeId  ,
                          IMAGEHLP_SYMBOL_TYPE_INFO  GetType ,
                          PVOID                      pInfo    ) ;
    
    BOOL SymEnumTypes ( ULONG64     BaseOfDll           ,
                        PSYM_ENUMERATESYMBOLS_CALLBACK
                                    EnumSymbolsCallback ,
                        PVOID       UserContext          ) ;
    
/*----------------------------------------------------------------------
                        Public Local Enumeration
----------------------------------------------------------------------*/
public      :

    BOOL EnumLocalVariables
                    ( PENUM_LOCAL_VARS_CALLBACK      pCallback     ,
                      int                            iExpandLevel  ,
                      BOOL                           bExpandArrays ,
                      PREAD_PROCESS_MEMORY_ROUTINE64 pReadMem      ,
                      LPSTACKFRAME64                 pFrame        ,
                      CONTEXT *                      pContext      ,
                      PVOID                          pUserContext   ) ;

/*----------------------------------------------------------------------
                        Public Symbol Manipulation
----------------------------------------------------------------------*/
public      :

#pragma warning(disable : 4996)

    BOOL SymEnumerateSymbols64 (IN DWORD64                    BaseOfDll,
                                IN PSYM_ENUMSYMBOLS_CALLBACK64
                                                    EnumSymbolsCallback,
                                IN PVOID                   UserContext )
    {
        return ( ::SymEnumerateSymbols64( m_hProcess          ,
                                           BaseOfDll           ,
                                           EnumSymbolsCallback ,
                                           UserContext          ) ) ;
    }
    
    BOOL SymEnumerateSymbolsW64 ( IN DWORD64                 BaseOfDll,
                                  IN PSYM_ENUMSYMBOLS_CALLBACK64W
                                                  EnumSymbolsCallback ,
                                  IN PVOID                 UserContext )
    {
        return ( ::SymEnumerateSymbolsW64 ( m_hProcess          ,
                                            BaseOfDll           ,
                                            EnumSymbolsCallback ,
                                            UserContext          ) ) ;
    }
    
#ifndef _WIN64
   BOOL SymEnumerateSymbols (IN DWORD64                    BaseOfDll,
                              IN PSYM_ENUMSYMBOLS_CALLBACK64
                                                  EnumSymbolsCallback,
                              IN PVOID                   UserContext )
    {
        return ( ::SymEnumerateSymbols64 ( m_hProcess          ,
                                           BaseOfDll           ,
                                           EnumSymbolsCallback ,
                                           UserContext          ) ) ;
    }
#endif

    BOOL SymGetSymFromAddr64 ( IN  DWORD64             dwAddr          ,
                               OUT PDWORD64            pdwDisplacement ,
                               OUT PIMAGEHLP_SYMBOL64  Symbol          )
    {
        return ( ::SymGetSymFromAddr64 ( m_hProcess       ,
                                         dwAddr           ,
                                         pdwDisplacement  ,
                                         Symbol            ) ) ;
    }

#ifndef _WIN64
    BOOL SymGetSymFromAddr ( IN  DWORD64             dwAddr          ,
                             OUT PDWORD64            pdwDisplacement ,
                             OUT PIMAGEHLP_SYMBOL64  Symbol          )
    {
        return ( ::SymGetSymFromAddr64 ( m_hProcess       ,
                                         dwAddr           ,
                                         pdwDisplacement  ,
                                         Symbol            ) ) ;
    }
 #endif

    BOOL SymGetSymFromName64 ( IN  LPSTR              Name   ,
                               OUT PIMAGEHLP_SYMBOL64 Symbol  )
    {
        return ( ::SymGetSymFromName64 ( m_hProcess ,
                                         Name       ,
                                         Symbol      ) ) ;
    }
/* Removed by Waj At [2012/8/13/]

    BOOL SymGetSymFromName ( IN  LPSTR              Name   ,
                             OUT PIMAGEHLP_SYMBOL64 Symbol  )
    {
        return ( ::SymGetSymFromName64 ( m_hProcess ,
                                         Name       ,
                                         Symbol      ) ) ;
    }
*/

    BOOL SymGetSymNext64 ( IN OUT PIMAGEHLP_SYMBOL64 Symbol )
    {
        return ( ::SymGetSymNext64 ( m_hProcess , Symbol ) ) ;
    }

#ifndef _WIN64
   BOOL SymGetSymNext ( IN OUT PIMAGEHLP_SYMBOL64 Symbol )
    {
        return ( ::SymGetSymNext64 ( m_hProcess , Symbol ) ) ;
    }
#endif

    BOOL SymGetSymPrev64 ( IN OUT PIMAGEHLP_SYMBOL64 Symbol )
    {
        return ( ::SymGetSymPrev64 ( m_hProcess , Symbol ) ) ;
    }

#ifndef _WIN64
   BOOL SymGetSymPrev ( IN OUT PIMAGEHLP_SYMBOL64 Symbol )
    {
        return ( ::SymGetSymPrev64 ( m_hProcess , Symbol ) ) ;
    }
#endif

/*----------------------------------------------------------------------
                     Public Source Line Manipulation
----------------------------------------------------------------------*/
public      :

    BOOL SymGetLineFromAddr64 ( IN  DWORD64          dwAddr          ,
                                OUT PDWORD           pdwDisplacement ,
                                OUT PIMAGEHLP_LINE64 Line             )
    {

        return ( ::SymGetLineFromAddr64 ( m_hProcess      ,
                                          dwAddr          ,
                                          pdwDisplacement ,
                                          Line             ) ) ;

    }

#ifndef _WIN64
    BOOL SymGetLineFromAddr ( IN  DWORD64          dwAddr          ,
                              OUT PDWORD           pdwDisplacement ,
                              OUT PIMAGEHLP_LINE64 Line             )
    {

        return ( ::SymGetLineFromAddr64 ( m_hProcess      ,
                                          dwAddr          ,
                                          pdwDisplacement ,
                                          Line             ) ) ;

    }
 #endif

    BOOL SymGetLineFromName64 ( IN     LPSTR            ModuleName    ,
                                IN     LPSTR            FileName      ,
                                IN     DWORD            dwLineNumber  ,
                                OUT    PLONG            plDisplacement,
                                IN OUT PIMAGEHLP_LINE64 Line           )
    {
        return ( ::SymGetLineFromName64 ( m_hProcess       ,
                                          ModuleName       ,
                                          FileName         ,
                                          dwLineNumber     ,
                                          plDisplacement   ,
                                          Line              ) ) ;
    }

#ifndef _WIN64
    BOOL SymGetLineFromName ( IN     LPSTR            ModuleName    ,
                              IN     LPSTR            FileName      ,
                              IN     DWORD            dwLineNumber  ,
                              OUT    PLONG            plDisplacement,
                              IN OUT PIMAGEHLP_LINE64 Line           )
    {
        return ( ::SymGetLineFromName64 ( m_hProcess       ,
                                          ModuleName       ,
                                          FileName         ,
                                          dwLineNumber     ,
                                          plDisplacement   ,
                                          Line              ) ) ;
    }
 #endif

    BOOL SymGetLineNext64 ( IN OUT PIMAGEHLP_LINE64 Line )
    {
        return ( ::SymGetLineNext64 ( m_hProcess , Line ) ) ;
    }

#ifndef _WIN64
   BOOL SymGetLineNext ( IN OUT PIMAGEHLP_LINE64 Line )
    {
        return ( ::SymGetLineNext64 ( m_hProcess , Line ) ) ;
    }
#endif

    BOOL SymGetLinePrev64 ( IN OUT PIMAGEHLP_LINE64 Line )
    {
        return ( ::SymGetLinePrev64 ( m_hProcess , Line ) ) ;
    }
#ifndef _WIN64
    BOOL SymGetLinePrev ( IN OUT PIMAGEHLP_LINE64 Line )
    {
        return ( ::SymGetLinePrev64 ( m_hProcess , Line ) ) ;
    }
#endif
    BOOL SymMatchFileName ( IN  LPSTR   FileName        ,
                            IN  LPSTR   Match           ,
                            OUT LPSTR * FileNameStop    ,
                            OUT LPSTR * MatchStop        )
    {
        return ( ::SymMatchFileName ( FileName       ,
                                      Match          ,
                                      FileNameStop   ,
                                      MatchStop       ) ) ;
    }

    BOOL SymEnumSourceFiles (IN ULONG64 ModBase         ,
                             IN LPSTR   Mask            ,
                             IN PSYM_ENUMSOURCFILES_CALLBACK cbSrcFiles,
                             IN PVOID   UserContext      )
    {
        return ( ::SymEnumSourceFiles ( m_hProcess  ,
                                        ModBase     ,
                                        Mask        ,
                                        cbSrcFiles  ,
                                        UserContext  ) ) ;
    }
    
/*----------------------------------------------------------------------
                       Public Miscellaneous Members
----------------------------------------------------------------------*/
public      :

    LPVOID SymFunctionTableAccess64 ( DWORD64 AddrBase )
    {
        return ( ::SymFunctionTableAccess64 ( m_hProcess , AddrBase ) ) ;
    }
#ifndef _WIN64
    LPVOID SymFunctionTableAccess ( DWORD64 AddrBase )
    {
        return ( ::SymFunctionTableAccess64 ( m_hProcess , AddrBase ) ) ;
    }
#endif
    BOOL SymGetSearchPath ( OUT LPSTR SearchPath        ,
                            IN  DWORD SearchPathLength   )
    {
        return ( ::SymGetSearchPath ( m_hProcess       ,
                                      SearchPath       ,
                                      SearchPathLength  ) ) ;
    }

    // John's wide wrapper.
    BOOL SymGetSearchPath ( OUT LPWSTR SearchPath        ,
                            IN  DWORD  SearchPathLength   ) ;

    BOOL SymSetSearchPath ( IN LPSTR SearchPath )
    {
        return ( ::SymSetSearchPath ( m_hProcess , SearchPath ) ) ;
    }
    
    // John's wide wrapper.
    BOOL SymSetSearchPath ( IN LPWSTR SearchPath ) ;
    
    // John's additional helper function.  Appends the path onto the
    // existing search path.
    BOOL SymAppendSearchPath ( IN LPCWSTR SearchPath ) ;
#ifndef _WIN64
    BOOL SymRegisterCallback ( IN PSYMBOL_REGISTERED_CALLBACK
                                                       CallbackFunction,
                               IN PVOID                UserContext    )
    {
        return ( ::SymRegisterCallback ( m_hProcess         ,
                                         CallbackFunction   ,
                                         UserContext         ) ) ;
    }
#else
	BOOL SymRegisterCallback ( IN PSYMBOL_REGISTERED_CALLBACK
									CallbackFunction,
								IN ULONG64                UserContext    )
	{
		return ( ::SymRegisterCallback ( m_hProcess         ,
										CallbackFunction   ,
										UserContext         ) ) ;
	}

#endif

    BOOL SymFindFileInPath ( LPSTR                      SearchPath  ,
                             LPSTR                      FileName    ,
                             PVOID                      id          ,
                             DWORD                      two         ,
                             DWORD                      three       ,
                             DWORD                      flags       ,
                             LPSTR                      FilePath    ,
                             PFINDFILEINPATHCALLBACK    callback    ,
                             PVOID                      context      )
    {
        return ( ::SymFindFileInPath ( m_hProcess  ,
                                       SearchPath  ,
                                       FileName    ,
                                       id          ,
                                       two         ,
                                       three       ,
                                       flags       ,
                                       FilePath    ,
                                       callback    ,
                                       context      ) ) ;
    }

/*----------------------------------------------------------------------
                       Public Completeness Members
----------------------------------------------------------------------*/
    DWORD SymGetOptions ( void )
    {
        return ( ::SymGetOptions ( ) ) ;
    }

    DWORD SymSetOptions ( DWORD SymOptions )
    {
        return ( ::SymSetOptions ( SymOptions ) ) ;
    }

    BOOL SymMatchString ( IN LPSTR string       ,
                          IN LPSTR expression   ,
                          IN BOOL  fCase         )
    {
        return ( ::SymMatchString ( string , expression , fCase ) ) ;
    }

    BOOL SymUnDName64 ( IN  PIMAGEHLP_SYMBOL64 sym              ,
                        OUT PSTR               UnDecName        ,
                        IN  DWORD              UnDecNameLength   )
    {
        return ( ::SymUnDName64 ( sym , UnDecName , UnDecNameLength ) ) ;
    }
#ifndef _WIN64
    BOOL SymUnDName ( IN  PIMAGEHLP_SYMBOL64 sym              ,
                      OUT PSTR               UnDecName        ,
                      IN  DWORD              UnDecNameLength   )
    {
        return ( ::SymUnDName64 ( sym , UnDecName , UnDecNameLength ) ) ;
    }
#endif
/*----------------------------------------------------------------------
                          Protected Data Methods
----------------------------------------------------------------------*/
protected   :
    
    // Initialized all stored values used for enumerating local
    // variables.
    void ZeroLocalEnumerationMemberVars ( void )
    {
        m_pUserCallback = NULL ;
        m_pFrame        = NULL ;
        m_pContext      = NULL ;
        m_pReadMem      = NULL ;
        m_pUserContext  = NULL ;
        m_dwCurrLocalModBase = 0 ;
        m_iMaxExpandLevel = 2 ;
        m_bExpandArrays = FALSE ;
    }
    
    // The friend function that takes care of the enumeration.
    friend BOOL CALLBACK LocalsEnumSymbols ( PSYMBOL_INFO pSymInfo    ,
                                             ULONG        SymbolSize  ,
                                             PVOID        UserContext);
                                             
    // The main decoder function.
    BOOL DecodeVariable ( CVariableInfo & cStartVar , int iIndent ) ;
    
    // Gets the real address of the variable.
    BOOL DetermineAddress ( CVariableInfo & cVar ) ;
#ifdef _M_IX86
    BOOL ConvertRegisterToValueIA32 ( CV_HREG_e eReg     ,
                                      DWORD64 & dwVal    ,
                                      CONTEXT * pContext  ) ;
#else
    BOOL ConvertRegisterToValueIA64 ( CV_HREG_e eReg     ,
                                      DWORD64 & dwVal    ,
                                      CONTEXT * pContext  ) ;
#endif
    
    // Decodes a single variable's type.
    BOOL DecodeType ( CVariableInfo & cStartVar ) ;
    
    // All the functions to help decode types.
    BOOL DecodeTypeSymTagBaseType ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeSymTagPointerType ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeSymTagUDT ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeTagArrayType ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeSymTagFunctionType ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeSymTagFunctionArgType ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeSymTagEnum ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeSymTagData ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeSymTagVTable ( CVariableInfo & cVar ) ;
    BOOL DecodeTypeSymTagBaseClass ( CVariableInfo & cVar ) ;
    
    // Decodes a variables value.
    BOOL DecodeValue ( CVariableInfo & cVar ) ;
    
    // All the functions to help decode values.
    BOOL DecodeValueSymTagBaseType ( CVariableInfo & cVar ) ;
    BOOL DecodeValueSymTagPointerType ( CVariableInfo & cVar ) ;
    BOOL DecodeValueSymTagEnum ( CVariableInfo & cVar ) ;
    BOOL DecodeValueSymTagArrayType ( CVariableInfo & cVar ) ;
    
    // Various helpers and wrappers.
    BOOL OutputVariable ( CVariableInfo & cVar , int iIndent ) ;
    
    // Reads memory based on the m_pMemRead value.
    BOOL MemRead ( LPCVOID lpAddr    ,
                   PBYTE   pData     ,
                   int     iDataSize ,
                   LPDWORD lpdwRead   ) ;
                   
    // Reads and updates the address if the tag is a pointer.
    BOOL ReadPointerAddress ( CVariableInfo & cVar ) ;
    
    // Dumps arrays.
    BOOL DumpArray ( CVariableInfo & cStartVar , int iIndent ) ;
    
    // Gets the specified export out of DBGHELP.DLL
    FARPROC GetDbgHelpAddress ( LPCSTR szFunc ) ;

/*----------------------------------------------------------------------
                          Protected Data Members
----------------------------------------------------------------------*/
protected   :
    // The unique value that will be used for this instance of the
    // symbol engine. This value doesn’t have to be an actual
    // process value, just a unique value.
    HANDLE      m_hProcess      ;
    
    
    // The stack, context, memory read routine user callback and user
    // context variables initialized on each call to EnumLocalVariables.
    PENUM_LOCAL_VARS_CALLBACK      m_pUserCallback ;
    LPSTACKFRAME64                 m_pFrame   ;
    CONTEXT *                      m_pContext ;
    PREAD_PROCESS_MEMORY_ROUTINE64 m_pReadMem ;
    PVOID                          m_pUserContext ;
    
    // How much variable expansion is supposed to go on.
    int m_iMaxExpandLevel ;
    // Set to true to expand arrays found at < m_iMaxExpandLevel.
    int m_bExpandArrays ;
    
    // The module base of the current local being decoded.
    DWORD64 m_dwCurrLocalModBase ;
    
    // The number radix to use with formatted local variables.
    int m_iRadix ;
    
    // The current local being manipulated.  I keep this at the class
    // level so I don't have to allocate and deallocate so much memory.
    CVariableInfo m_cCurrentLocal ;

    // The functions that stock W2K's DBGHELP.DLL does not support so
    // I can't call them directly.
    PSYMENUMSYMBOLS     m_pSymEnumSymbols ;
    PSYMSETCONTEXT      m_pSymSetContext ;
    PSYMFROMADDR        m_pSymFromAddr ;
    PSYMFROMNAME        m_pSymFromName ;
    PSYMGETTYPEFROMNAME m_pSymGetTypeFromName ;
    PSYMGETTYPEINFO     m_pSymGetTypeInfo ;
    PSYMENUMTYPES       m_pSymEnumTypes ;

} ;



#endif      // _SYMBOLENGINE_H
