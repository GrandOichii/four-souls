#include "util.hpp"

#include <iostream>

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

void dumpstack (lua_State *L) {
    int top=lua_gettop(L);
    for (int i=1; i <= top; i++) {
        std::cout << i << "\t" << luaL_typename(L,i) << "\t";
        switch (lua_type(L, i)) {
        case LUA_TNUMBER:
            std::cout << lua_tonumber(L,i) << std::endl;
            break;
        case LUA_TSTRING:
            std::cout << lua_tostring(L,i) << std::endl;
            break;
        case LUA_TBOOLEAN:
            std::cout << lua_toboolean(L,i) << std::endl;
            break;
        case LUA_TNIL:
            std::cout << "nil" << std::endl;
            break;
        default:
            std::cout << lua_topointer(L,i) << std::endl;
            break;
        }
    }
}