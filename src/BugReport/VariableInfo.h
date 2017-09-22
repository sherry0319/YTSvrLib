/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright ?1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _DECODEDTYPE_H
#define _DECODEDTYPE_H

#include "JString.h"

#include "CVConst.h"

// The maximum variable name.
#define MAX_VARNAME MAX_PATH

class CVariableInfo
{
/*//////////////////////////////////////////////////////////////////////
Public Constructors and Destructors.
//////////////////////////////////////////////////////////////////////*/
public      :
    CVariableInfo ( void )
    {
        ZeroData ( ) ;
    }
    
    // Initializes the class from a SYMBOL_INFO class.
    CVariableInfo ( PSYMBOL_INFO pSymInfo , enum SymTagEnum eStartTag )
    {
        dwAddress = pSymInfo->Address ;
        iVariableLen = pSymInfo->Size ;
        eTag = eStartTag ;
        eDecodeTypeTag = eStartTag ;
        ulTypeIndex = pSymInfo->TypeIndex ;
        ulDecodeTypeIndex = ulTypeIndex ;
        ulFlags = pSymInfo->Flags ;
        ulValue = pSymInfo->Value ;
        ulRegister = pSymInfo->Register ;
        
        eBT = btNoType ;
        ulBTLen = 0 ;
        
        dwArrayElemCount = 0 ;
        bIsStringArray = FALSE ;

        if ( '\0' != pSymInfo->Name[ 0 ] )
        {
#ifdef UNICODE        
            MultiByteToWideChar ( CP_THREAD_ACP     ,
                                  0                 ,
                                  pSymInfo->Name    ,
                                  -1                ,
                                  szVarName         ,
                                  MAX_VARNAME - 1    ) ;
            szVarName[ MAX_VARNAME - 1 ] = _T ( '\0' ) ;
#else
			_tcscpy_s(szVarName, MAX_VARNAME, pSymInfo->Name);
#endif            
        }
    }
    
    virtual ~CVariableInfo ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
Public Methods
//////////////////////////////////////////////////////////////////////*/
public      :
    void ZeroData ( void )
    {
        dwAddress = 0 ;
        iVariableLen = 0 ;
        eTag = SymTagNull ;
        eDecodeTypeTag = SymTagNull ;
        ulTypeIndex = 0 ;
        ulDecodeTypeIndex = 0 ;
        ulFlags = 0 ;
        ulValue = 0 ;
        ulRegister = 0 ;
        
        eBT = btNoType ;
        ulBTLen = 0 ;
        
        dwArrayElemCount = 0 ;
        bIsStringArray = FALSE ;
        
        szVarName[ 0 ] = _T ( '\0' ) ;
        cTypeName.NullString ( ) ;
        szValue[ 0 ] = _T ( '\0' ) ;
    }
    
    void InitializeFromSymbolInfo ( PSYMBOL_INFO    pSymInfo  ,
                                    enum SymTagEnum eStartTag  )
    {
        dwAddress = pSymInfo->Address ;
        iVariableLen = pSymInfo->Size ;
        eTag = eStartTag ;
        eDecodeTypeTag = eStartTag ;
        ulTypeIndex = pSymInfo->TypeIndex ;
        ulDecodeTypeIndex = ulTypeIndex ;
        ulFlags = pSymInfo->Flags ;
        ulValue = pSymInfo->Value ;
        ulRegister = pSymInfo->Register ;
        
        eBT = btNoType ;
        ulBTLen = 0 ;
        
        dwArrayElemCount = 0 ;
        bIsStringArray = FALSE ;

        szVarName[ 0 ] = _T ( '\0' ) ;
        cTypeName.NullString ( ) ;
        szValue[ 0 ] = _T ( '\0' ) ;

        if ( '\0' != pSymInfo->Name[ 0 ] )
        {
#ifdef UNICODE        
            MultiByteToWideChar ( CP_THREAD_ACP     ,
                                  0                 ,
                                  pSymInfo->Name    ,
                                  -1                ,
                                  szVarName         ,
                                  MAX_VARNAME - 1    ) ;
            szVarName[ MAX_VARNAME - 1 ] = _T ( '\0' ) ;
#else
			_tcscpy_s(szVarName, MAX_VARNAME, pSymInfo->Name);
#endif            
        }

    }


/*//////////////////////////////////////////////////////////////////////
Public Member Data
//////////////////////////////////////////////////////////////////////*/
public      :
    // The address for this variable.
    DWORD64 dwAddress ;
    
    // The decoded variable name.
    TCHAR szVarName [ MAX_VARNAME ] ;
    
    // The decoded type name.
    JString cTypeName ;
    
    // The decoded value.
    TCHAR szValue[ MAX_PATH ] ;
    
    // The length of the variable in bytes.
    int iVariableLen ;
    
    // The original sym type.
    enum SymTagEnum eTag ;
    
    // The original type index.
    ULONG ulTypeIndex ;
    
    // The field used for decoding the type.  This gets whacked as the
    // class is passed through the recursive decoding.
    enum SymTagEnum eDecodeTypeTag ;
    
    // The type index.  This field gets whacked as this class is passed
    // through the recursive decoding.
    ULONG ulDecodeTypeIndex ;
    
    // Any flags for this symbol.  These flags correspond to the
    // SYM_INFO flags.
    ULONG ulFlags ;
    
    // Value of a constant if flags contains
    // IMAGEHLP_SYMBOL_INFO_VALUEPRESENT.
    ULONG64 ulValue ;
    
    // If ulFlags contains a IMAGEHLP_SYMBOL_INFO_REGISTER, holds
    // the register.
    ULONG ulRegister ;
    
    // To keep from looking up the SymTagBaseType information multiple
    // times, these fields will be filled out if this variable is a
    // basic type.
    BasicType eBT ;
    ULONG64   ulBTLen ;
    
    // For arrays, this holds the element count.
    DWORD dwArrayElemCount ;
    // For string arrays, I won't expand them in the normal processing.
    BOOL  bIsStringArray ;
    
} ;

#endif  // _DECODEDTYPE_H
