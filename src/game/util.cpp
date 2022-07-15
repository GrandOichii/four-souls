#include "util.hpp"

#include <iostream>

#include "cards.hpp"

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

void l_pushtableboolean(lua_State* L, string key, bool value) {
    lua_pushstring(L, key.c_str());
    lua_pushboolean(L, value);
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

// string toJson(const string& v) { return "\"" + v + "\""; }
// string toJson(const int& v) { return std::to_string(v); }
// string toJson(const bool& v) { return (v ? "true" : "false"); }

// string toJson(const CardState& state) {
//     string result = "{";
//     addValue(result, "active", state.active);
//     addValue(result, "id", state.id);
//     addValue(result, "cardName", state.cardName);
//     addValue(result, "counters", state.counters, true);
//     result += "}";
//     return result;
// }

// template<class T>
// string toJson(std::vector<T> v) {
//     string result = "[";
//     for (int i = 0; i < v.size(); i++) {
//         result += toJson(v[i]);
//         if (i != v.size()-1) result += ", ";
//     }
//     result += "]";
//     return result;
// }

// template<class T>
// void addValue(string& j, string key, T value, bool last) {
//     j += "\"" + key + "\" : " + toJson(value) + "" + (last ? "" : ", ");
// }

// template<class T>
// void addValueV(string& j, string key, std::vector<T> value, bool last) {
//     j += "\"" + key + "\" : [";
//     for (int i = 0; i < value.size(); i++) {
//         j += toJson(value[i]);
//         if (i != value.size()-1) j += ", ";
//     }
//     j += "]";
//     if (last) j += "}";
// }