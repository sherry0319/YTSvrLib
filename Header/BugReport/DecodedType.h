/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _DECODEDTYPE_H
#define _DECODEDTYPE_H

#include "JString.h"

class CDecodedType
{
/*//////////////////////////////////////////////////////////////////////
Public Constructors and Destructors.
//////////////////////////////////////////////////////////////////////*/
public      :
    CDecodedType ( void )
    {
        dwAddress = 0 ;
        iVariableLen = 0 ;
        eDecodeTag = SymTagNull ;
        ulTypeIndex = 0 ;
        dwModBase = 0 ;
        ulFlags = 0 ;
        ulValue = 0 ;
        ulRegister = 0 ;
        
    }
    
    // Initializes the class from a SYMBOL_INFO class.
    CDecodedType ( PSYMBOL_INFO pSymInfo )
    {
        dwAddress = pSymInfo->Address ;
        iVariableLen = pSymInfo->Size ;
        eDecodeTag = (enum SymTagEnum)pSymInfo->Tag ;
        ulTypeIndex = pSymInfo->TypeIndex ;
        dwModBase = pSymInfo->ModBase ;
        ulFlags = pSymInfo->Flags ;
        ulValue = pSymInfo->Value ;
        ulRegister = pSymInfo->Register ;
        
        if ( '\0' != pSymInfo->Name[ 0 ] )
        {
            _tcsncpy ( cVar.szVarName , szwTypeName , MAX_VARNAME ) ;
            cVar.szVarName[ MAX_VARNAME - 1 ] = _T ( '\0' ) ;
        }
    }
    
    virtual ~CDecodedType ( void )
    {
    }

/*//////////////////////////////////////////////////////////////////////
Public Member Data
//////////////////////////////////////////////////////////////////////*/
public      :
    // The address for this variable.
    DWORD64 dwAddress ;
    
    // The variable name of the type.
    JString cVarName ;
    
    // The type name of the type.
    JString cTypeName ;
    
    // The length of the variable in bytes.
    int iVariableLen ;
    
    // The sym type.  This field gets whacked as this class is passed
    // through the recursive decoding.
    enum SymTagEnum eDecodeTag ;
    
    // The type index.  This field gets whacked as this class is passed
    // through the recursive decoding.
    ULONG ulTypeIndex ;
    
    // The module base for this symbol.
    DWORD64 dwModBase ;
    
    // Any flags for this symbol.  These flags correspond to the
    // SYM_INFO flags.
    ULONG ulFlags ;
    
    // Value of a constant if flags contains
    // IMAGEHLP_SYMBOL_INFO_VALUEPRESENT.
    ULONG64 ulValue ;
    
    // If ulFlags contains a IMAGEHLP_SYMBOL_INFO_REGISTER, holds
    // the register.
    ULONG ulRegister ;

    
    
} ;

#endif  // _DECODEDTYPE_H
