#pragma once

#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>
#include <random>
#include <stdlib.h>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

#include "cards.hpp"

using std::string;

const int MIN_PLAYER_COUNT = 2;
const int MAX_PLAYER_COUNT = 4;
const int SOULS_TO_WIN = 4;

class Player {
private:
    std::string _name;

    int _baseMHealth;
    int _health;

    int _baseAttack;
public:
    Player(std::string name, CharacterCard* card) :
        _name(name) 
    {
        this->_baseMHealth = card->health();
        this->_health = this->_baseMHealth;

        this->_baseAttack = card->attack();
        //  TODO parse character card
    }

    ~Player() {

    }

    string name() { return _name; }
};

class Match {
private:
    lua_State *L;


    std::default_random_engine rng = std::default_random_engine {};
    std::vector<std::pair<CharacterCard*, bool>> _characterPool;

    std::vector<Player*> _players;
    int _currentPlayer;

    std::deque<LootCard*> _lootDeck;
    std::deque<TrinketCard*> _treasureDeck;
public:
    Match() {
        this->rng.seed(rand());
        this->setupLua();
    }

    ~Match() {
        for (const auto& p : _players) delete p;

        lua_close(this->L);
    }

    int luafunc_cfunc(lua_State *L) {
        auto f = (float)lua_tonumber(L, -1);
        lua_pushnumber(L, f * 10);
        return 1; // the amount of return values
    }

    void setupLua() {
        this->L = luaL_newstate();
        // connect common libs
        luaL_openlibs(L);
        // connect functions
        lua_register(L, "cfunc", this->luafunc_cfunc);
        // std::map<string, std::function<int(lua_State*)>> funcMap = {
            
        // };
        // for (const auto& [key, value] : funcMap)
        //     lua_register(L, key.c_str(), value);
    }

    void lua_err() {
        string errormsg = lua_tostring(this->L, -1);
        std::cout << errormsg << std::endl;
    }

    void execScript(string script) {
        int r = luaL_dostring(L, script.c_str());
        if (r != LUA_OK) {
            lua_err();
            exit(1);
        }
    }

    void addToCharacterPool(CharacterCard* card) {
        _characterPool.push_back(std::make_pair(card, true));
    }

    Player* addPlayer(std::string name, CharacterCard* character) {
        for (const auto& p : _players)
            if (p->name() == name)
                return nullptr;
        auto result = new Player(name, character);
        _players.push_back(result);
        return result;
    }

    std::vector<CharacterCard*> getAvailableCharacters() {
        std::vector<CharacterCard*> result;
        for (const auto& pair : _characterPool)
            if (pair.second)
                result.push_back(pair.first);
        return result;
    }

    CharacterCard* getRandomAvailableCharacter() {
        auto characters = this->getAvailableCharacters();
        int top = characters.size();
        int i = rand() % top;
        auto result = characters[i];
        for (auto& pair : _characterPool) {
            if (pair.first == result) {
                pair.second = false;
                break;
            }
        }
        return result;
    }

    void shuffleLootDeck() {
        std::shuffle(std::begin(_lootDeck), std::end(_lootDeck), rng);
    }

    void createLootDeck(std::vector<std::pair<LootCard*, int>> pairs) {
        for (const auto& pair : pairs)
            for (int i = 0; i < pair.second; i++)
                this->_lootDeck.push_back(pair.first);
        this->shuffleLootDeck();
    }

    void start() {
        for (const auto& card : _lootDeck)
            std::cout << card->name() << std::endl;
    }
};