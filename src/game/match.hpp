#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <map>
#include <stack>
#include <random>
#include <stdlib.h>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

#include "cards.hpp"
#include "types.hpp"

using std::string;

const int MIN_PLAYER_COUNT = 2;
const int MAX_PLAYER_COUNT = 4;
const int SOULS_TO_WIN = 4;

static void l_pushtablestring(lua_State* L, string key, string value) {
    lua_pushstring(L, key.c_str());
    lua_pushstring(L, value.c_str());
    lua_settable(L, -3);
}

static void l_pushtablenumber(lua_State* L, string key, float value) {
    lua_pushstring(L, key.c_str());
    lua_pushnumber(L, value);
    lua_settable(L, -3);
}

class Player {
private:
    std::string _name;

    int _baseMHealth;
    int _health;

    int _baseAttack;
    int _id;

    bool _characterActive;
    std::vector<CardWrapper*> _board;
public:
    Player(std::string name, CharacterCard* card, int id) :
        _name(name),
        _id(id) 
    {
        this->_baseMHealth = card->health();
        this->_health = this->_baseMHealth;

        this->_baseAttack = card->attack();
        this->_board.push_back(new CardWrapper(card->startingItem(), _id));
        this->_characterActive = false;
    }

    ~Player() {
        for (const auto& w : _board) delete w;
    }

    string name() { return _name; }

    void print() {
        std::cout << _name << " (" << _id << ")" << std::endl;
        std::cout << "Board: " << std::endl;
        for (const auto& w : _board)
            std::cout << w->card()->name() << " (" << w->id() << ")" << std::endl;
    }

    std::vector<CardWrapper*> board() { return _board; }

    bool characterActive() { return _characterActive; }

    void rechargeCharacter() {
        this->_characterActive = true;
    }

    void rechargeCards() {
        for (auto& w : _board)
            w->recharge();
    }
    
    void recharge() {
        this->rechargeCharacter();
        this->rechargeCards();
    }

    void pushTable(lua_State* L) {
        lua_newtable(L);
        l_pushtablestring(L, "name", this->_name);
        l_pushtablenumber(L, "id", (float)this->_id);
    }
};

struct StackEffect {
    string funcName;
    Player* player;
    CardWrapper* card;
    StackEffect(string funcName, Player* player, CardWrapper* card) :
        funcName(funcName),
        player(player),
        card(card) {}
};

class Match {
private:
    bool _running = false;
    lua_State *L;

    std::default_random_engine rng = std::default_random_engine {};
    std::vector<std::pair<CharacterCard*, bool>> _characterPool;

    std::vector<Player*> _players;
    int _currentI;
    int _nextI;
    int _priorityI;
    Player* _activePlayer;

    std::deque<LootCard*> _lootDeck;
    std::deque<TrinketCard*> _treasureDeck;

    std::stack<StackEffect> _stack;
public:
    Match() {
        this->rng.seed(rand());
    }

    ~Match() {
        for (const auto& p : _players) delete p;

        lua_close(this->L);
    }

    static int wrap_GetOwner(lua_State *L) {
        if (lua_gettop(L) != 1) {
            lua_err(L);
            exit(1);
        }
        auto match = static_cast<Match*>(lua_touserdata(L, 1));
        match->_stack.top().player->pushTable(L);
        return 1;
    }

    // int luafunc_cfunc(lua_State *L) {
    //     auto f = (float)lua_tonumber(L, -1);
    //     lua_pushnumber(L, f * 10);
    //     return 1; // the amount of return values
    // }

    // void foo() {
    //     std::cout << "Player count: " << this->_players.size() << std::endl;
    // }

    // static int wrap_foo(lua_State *L) {
    //     std::cout << lua_gettop(L) << std::endl;
    //     if (lua_gettop(L) != 1) {
    //         lua_err(L);
    //         exit(1);
    //     }
    //     // std::cout << lua_gettop(L) << std::endl;
    //     auto match = static_cast<Match*>(lua_touserdata(L, 1));
    //     match->foo();
    //     return 0;
    // }

    void setupLua() {
        this->L = luaL_newstate();
        // connect common libs
        luaL_openlibs(L);
        // connect functions
        // lua_register(L, "foo", wrap_foo);
        lua_register(L, "getOwner", wrap_GetOwner);

        // load card scripts
        for (const auto& card : _lootDeck) {
            std::cout << "Loading script for " << card->name() << std::endl;
            this->execScript(card->script());
        }
        for (const auto& card : _treasureDeck) {
            std::cout << "Loading script for " << card->name() << std::endl;
            this->execScript(card->script());
        }
        for (const auto& pair : _characterPool) {
            auto character = pair.first;
            auto card = character->startingItem();
            std::cout << "Loading script for " << card->name() << std::endl;
            this->execScript(card->script());
        }
        std::cout << "All scripts loaded!" << std::endl;
    }

    static void lua_err(lua_State *L) {
        string errormsg = lua_tostring(L, -1);
        std::cout << errormsg << std::endl;
    }

    void execScript(string script) {
        int r = luaL_dostring(L, script.c_str());
        if (r != LUA_OK) {
            lua_err(this->L);
            exit(1);
        }
    }

    void execFunc(string funcName) {
        lua_getglobal(L, funcName.c_str());
        if (!lua_isfunction(L, -1)) {
            lua_err(L);
            exit(1);
        }
        lua_pushlightuserdata(L, this);
        int r = lua_pcall(L, 1, 0, 0);
        if (r != LUA_OK) {
            lua_err(this->L);
            exit(1);
        }
    }

    bool execCheck(string funcName) {
        lua_getglobal(L, funcName.c_str());
        if (!lua_isfunction(L, -1)) {
            lua_err(L);
            exit(1);
        }
        lua_pushlightuserdata(L, this);
        int r = lua_pcall(L, 1, 1, 0);
        if (r != LUA_OK) {
            lua_err(this->L);
            exit(1);
        }
        if (!lua_isboolean(L, -1)) {
            lua_err(this->L);
            exit(1);
        }
        return (bool)lua_toboolean(L, -1);
    }

    void addToCharacterPool(CharacterCard* card) {
        _characterPool.push_back(std::make_pair(card, true));
    }

    Player* addPlayer(std::string name, CharacterCard* character) {
        for (const auto& p : _players)
            if (p->name() == name)
                return nullptr;
        auto result = new Player(name, character, _players.size()+1);
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

    void shuffleTreasureDeck() {
        std::shuffle(std::begin(_treasureDeck), std::end(_treasureDeck), rng);
    }

    void createLootDeck(std::vector<std::pair<LootCard*, int>> pairs) {
        for (const auto& pair : pairs)
            for (int i = 0; i < pair.second; i++)
                this->_lootDeck.push_back(pair.first);
        this->shuffleLootDeck();
    }

    void createTreasureDeck(std::vector<TrinketCard*> cards) {
        for (const auto& card : cards)
            this->_treasureDeck.push_back(card);            
        this->shuffleTreasureDeck();
    }

    void start() {
        std::cout << "\nThe game starts\n\n";
        // this->execFunc("TestFunc");

        std::cout << "Loot deck:" << std::endl;
        for (const auto& card : _lootDeck)
            std::cout << "\t" << card->name() << std::endl;
        std::cout << "Treasure deck:" << std::endl;
        for (const auto& card : _treasureDeck)
            std::cout << "\t" << card->name() << std::endl;
        std::cout << "Players:" << std::endl;
        for (const auto& p : _players)
            p->print();

        this->_currentI = rand() % _players.size();
        this->_priorityI = this->_currentI;
        this->_running = true;

        int c = 10;
        while (this->_running) {
            this->calcNext();
            this->turn();
            this->passTurn();
            c--;
            if (!c) break;
        }   
    }

    void passTurn() {
        this->_currentI = this->_nextI;
    }

    void calcNext() {
        this->_nextI = (this->_currentI + 1) % _players.size();
    }

    void turn() {
        this->_activePlayer = this->_players[this->_currentI];
        this->log(this->_activePlayer->name() + " starts their turn");
        // recharge all of the items and character card of the active player
        this->_activePlayer->recharge();
        // apply all <start of turn> triggers, then prompt actions
        this->applyTriggers(TURN_START_TRIGGER);
        this->resolveStack();
    }

    void applyTriggers(string triggerType) {
        // first the monsters

        // then the items, starting with the current player
        //  TODO prompt the player to order them
        for (int i = 0; i < _players.size(); i++) {
            auto player = _players[(_currentI + i) % _players.size()];
            auto board = player->board();
            for (const auto& w : board) {
                auto card = (TrinketCard*)w->card();
                if (!card->hasTrigger(triggerType)) continue;
                // std::cout << "Card " << card->name() << "[" << w->id() << "] has a " << triggerType << " trigger" << std::endl;
                auto pair = card->getTriggerWhen(triggerType);
                auto checkFuncName = pair.first;
                if (!this->execCheck(checkFuncName)) continue;
                this->log(card->name() + " is triggered.");
                this->_stack.push(StackEffect(pair.second, player, w));
            }
        }
    }

    void resolveStack() {
        // while the stack isn't empty
        while (!this->_stack.empty()) {
            this->resolveTop();
        }
    }

    void resolveTop() {
        auto effect = this->_stack.top();
        // pass the priority of the owner of the effect
        this->_priorityI = -1;
        for (int i = 0; i < _players.size(); i++) {
            if (_players[i] == effect.player) {
                this->_priorityI = i;
                break;
            }
        }
        const auto last = this->_priorityI;
        if (this->_priorityI == -1) throw std::runtime_error("Unknown player " + effect.player->name());
        do {
            // prompt action
            auto response = this->promptPlayerWithPriority();
            if (response == PASS_RESPONSE) {
                // player passes priority
                this->log(this->_players[this->_priorityI]->name() + " passes priority");
                this->_priorityI = (this->_priorityI + 1) % _players.size();
                continue;
            }
            // player put something on stack
            return;
        } while (last != this->_priorityI);
        // resolve the ability
        this->execFunc(effect.funcName);
        this->_stack.pop();
    }

    string promptPlayerWithPriority() {
        auto player = this->_players[this->_priorityI];
        this->log("Player " + player->name() + " takes priority");
        return PASS_RESPONSE;
    }

    void log(string message) {
        std::cout << " - " << message << std::endl;
    }
};