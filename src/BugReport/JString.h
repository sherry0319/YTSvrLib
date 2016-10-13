/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright ?1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JSTRING_H
#define _JSTRING_H

// A simple string class.
class JString
{
public   :

    // Constructor that creates a NULL string.
    JString ( void ) ;

    // Constructor that creates a string from a constant char array.
    JString ( const TCHAR * szInString ) ;
    
    // Constructor that creates a class that does not delete the
    // buffer when the constructor is called.
    JString ( bool bDeleteBufferOnDestruct ) ;

    // Copy constructor.
    JString ( const JString & cInString ) ;

    // Destructor.
    ~JString ( void ) ;

    // Equality operator. All compares are case insensitive.
    friend bool operator == ( const JString & s , const JString & t ) ;
    friend bool operator == ( const JString & s , const TCHAR * szBuff) ;

    // Inequality operator. All compares are case insensitive.
    friend bool operator != ( const JString & s , const JString & t ) ;
    friend bool operator != ( const JString & s , const TCHAR * szBuff) ;

    // Assignement operator.
    JString & operator = ( const JString & t ) ;
    JString & operator = ( const TCHAR * szStr ) ;
    JString & operator = ( const TCHAR cChar ) ;
    
#ifdef UNICODE
    JString & operator = ( const char * szAnsiStr ) ;
#endif  // UNICODE

    JString & operator += ( const JString & s ) ;

    JString & operator += ( const TCHAR * s ) ;

    JString & operator += ( const TCHAR c ) ;

#ifdef UNICODE
    JString & operator += ( const char * szAnsiStr ) ;
#endif  // UNICODE

    // Length function.
    size_t Length ( void ) const ;

    // Operator to allow us to use the JString in API functions.
    operator const TCHAR * ( void ) const ;

    // Concatination operator.
    friend JString operator + ( const JString & s ,
                                const JString & t  ) ;

    // Array operator.
    TCHAR & operator [] ( int iIndex ) ;

    // A check for empty strings.
    bool IsEmpty ( void ) ;

    // Forces the string to lowercase.
    void ToLower ( void ) ;

    // Trims any trialing whitespace.
    void TrimTrailingWhiteSpace ( void ) ;

    // Clears any data.
    void Empty ( void ) ;
    
    // Safely puts a NULL at the beginning of the string.
    void NullString ( void ) ;

    // A printf style filler.
    int Format ( const TCHAR * szFmt , ... ) ;

#ifndef _PORTABLE_JSTRING
    // Loads a string from a resource.  Call JStringSetResInst before
    // using this method.
    BOOL LoadString ( UINT uiResID ) ;
#endif  // _PORTABLE_JSTRING

    // Forces the buffer to a new length in characters.  If the buffer
    // is larger, the larger value is kept.
    void SetBufferLen ( size_t iNewSize ) ;

    // Get's the raw data buffer so API functions can fill it.  Make
    // sure to set the size with SetBufferLen first!
    TCHAR * GetBuffer ( void ) ;
    
    // Deallocates the buffer returned by GetBuffer.  As JString
    // implementations can change, always use this function to
    // deallocate returned buffers!!
    static void DeallocateBuffer ( TCHAR * szPreviousBuffer ) ;
    
    // Allows you to set the minimum allocation length.  This is
    // convenient if you are going to be adding a lot to this string.
    int SetMinumumAllocationLength ( int iNewLen ) ;
    
    // Tells the instance NOT to delete the underlying buffer.  You must
    // call GetBuffer and call delete [] on it yourself!!
    BOOL DoNotDeleteBuffer ( bool bDeleteBuffer ) ;

private  :
    // The data buffer.
    TCHAR * m_szData ;
    // The pointer to the end of the current string.
    TCHAR * m_pEndPos ;
    // The total amount of memory extra.
    size_t  m_iBuffLen ;
    // The allocation length.
    int     m_iAllocLen ;
    // If true, the buffer is not deleted in the destructor.  This
    // property is not copied over when copying JStrings.  You must
    // specifically call DoNotDeleteBuffer or construct the string
    // using the BOOL param constructor.
    bool    m_bDeleteBufferOnDestruct ;

} ;

#endif  // _JSTRING_H


