#pragma once

#include <string>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

using std::string;

void l_pushtablestring(lua_State* L, string key, string value);
void l_pushtablenumber(lua_State* L, string key, float value);
void l_pushtableboolean(lua_State* L, string key, bool value);
void dumpstack(lua_State *L);