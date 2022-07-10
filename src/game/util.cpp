#include "util.hpp"

void l_pushtablestring(lua_State* L, string key, string value) {
    lua_pushstring(L, key.c_str());
    lua_pushstring(L, value.c_str());
    lua_settable(L, -3);
}

void l_pushtablenumber(lua_State* L, string key, float value) {
    lua_pushstring(L, key.c_str());
    lua_pushnumber(L, value);
    lua_settable(L, -3);
}
