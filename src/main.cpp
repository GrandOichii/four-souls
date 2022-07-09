#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>  

#include "game/core.hpp"

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

using std::cout;
using std::endl;
using std::string;
using std::vector;

int luafunc_cfunc(lua_State *L) {
    auto f = (float)lua_tonumber(L, -1);
    lua_pushnumber(L, f * 10);
    return 1; // the amount of return values
}

int main() {
    // srand(time(0));
    srand(0);
    auto game = new Game("game");
    auto match = game->createMatch();
    auto p1 = match->addPlayer("oichii1", match->getRandomAvailableCharacter());
    auto p2 = match->addPlayer("oichii2", match->getRandomAvailableCharacter());
    match->start();
    delete game;
    // return 0;

    // lua_State *L = luaL_newstate();
    // luaL_openlibs(L);
    // lua_register(L, "cfunc", luafunc_cfunc);

    // int r = luaL_dofile(L, "testscript.lua");
    // if (r != LUA_OK) {
    //     lua_err(L);
    //     return -1;
    // }

    // r = luaL_dofile(L, "test.lua");
    // if (r != LUA_OK) {
    //     lua_err(L);
    //     return -1;
    // }

    

    // lua_getglobal(L, "funny");
    // if (!lua_isfunction(L, -1)) {
    //     lua_err(L);
    //     return -1;
    // }
    // lua_pushnumber(L, 4.5f);
    // lua_pushnumber(L, 23.f);
    // r = lua_pcall(L, 2, 0, 0);
    // if (r != LUA_OK) {
    //     lua_err(L);
    //     return -1;
    // }
    
    // lua_getglobal(L, "a");
    // if (!lua_isnumber(L, -1)) {
    //     lua_err(L);
    //     return -1;
    // }
    // float result = (float)lua_tonumber(L, -1);
    // cout << "a = " << result << endl;
    // lua_close(L);
}