#pragma once

#include <string>
#include <iostream>

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

template<class T, class cT>
bool removeFromCollection(T card, cT& cards) {
    if (std::find(cards.begin(), cards.end(), card) == cards.end()) return false;
    for (auto it = cards.begin(); it != cards.end(); it++) {
        if (*it == card) {
            cards.erase(it);
            break;
        }
    }
    return true;
}

// string toJson(const string& v);
// string toJson(const int& v);
// string toJson(const bool& v);

// struct CardState;
// string toJson(const CardState& state);

// template<class T>
// string toJson(std::vector<T> v);

// template<class T>
// void addValue(string& j, string key, T value, bool last=false);
// template<class T>
// void addValueV(string& j, string key, std::vector<T> value, bool last=false);
