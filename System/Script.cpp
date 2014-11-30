#include "windows.h"
#include "stdio.h"
#include "Script.h"
#include "core.h"
#include <cassert>

extern "C" {
#include "External\Lua-5.2.2\src\lua.h"
#include "External\Lua-5.2.2\src\lualib.h"
#include "External\Lua-5.2.2\src\lauxlib.h"
}

lua_State* L;

int ScriptPrint(lua_State* l)
{
	int top = lua_gettop(L);
	const char* pString = lua_tostring(L, -1);
	dpf(pString);
	return 0;
}

int ScriptTest(lua_State* L)
{
	return 0;
}

static const struct luaL_Reg ScriptLib [] = {
      {"print", ScriptPrint},
      {"test", ScriptTest},
      {NULL, NULL}  /* sentinel */
};

void ScriptStartup(const char* filename)
{
	L = luaL_newstate();
	luaL_openlibs(L);
	
	lua_pushcfunction(L, ScriptPrint);
	lua_setglobal(L, "print");
	
	luaL_newlib(L, ScriptLib);
	lua_setglobal(L, "Script");

	int error = luaL_loadfile(L, filename);
	if (error)
	{
		dpf("%s\n", lua_tostring(L, -1));
		assert(0);
	}

	lua_pcall(L, 0, 0, 0);
}

void ScriptUpdate(float dt)
{
	lua_getglobal(L, "Update");
	lua_pushnumber(L, dt);
	lua_pcall(L, 1, 0, 0);
}

void ScriptShutdown()
{
	lua_close(L);
}

