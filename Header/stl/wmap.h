#pragma once

typedef CMap<std::wstring, std::wstring&,  void*,void*> CMapWStringToPtr;
template<class   ARG_KEY>
AFX_INLINE UINT AFXAPI HashKey(std::wstring key)
{
	register size_t _Val = 5381;
	if( key.empty() )
		return (UINT)_Val;
	// 	wchar_t* _Begin = (wchar_t*)key.c_str();
	// 	while(*_Begin)
	// 		_Val = ((_Val << 5) + _Val) + (size_t)*_Begin++;
	for( size_t i=0; i<key.size(); i++ )
		_Val = ((_Val << 5) + _Val) + (size_t)key[i];
	return (UINT)(_Val);
}