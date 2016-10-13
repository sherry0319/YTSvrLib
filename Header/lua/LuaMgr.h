#pragma once
#ifndef _LUA_MGR_H
#define _LUA_MGR_H

#include "LuaBridge.h"

class CLuaMgr
{
public:
	CLuaMgr()
	{
		m_pLuaCore = luaL_newstate();
	}

	virtual ~CLuaMgr()
	{

	}

	lua_State* GetCurLuaState() {return m_pLuaCore;}

	luabridge::Namespace getGlobalNamespace() { return luabridge::getGlobalNamespace(m_pLuaCore);}

	void Run(const char* pfile) {luaL_dofile(m_pLuaCore,pfile);}
protected:
	lua_State* m_pLuaCore;
};

#endif