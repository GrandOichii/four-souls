#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
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

#include "player.hpp"
#include "cards.hpp"
#include "types.hpp"
#include "util.hpp"

using std::string;

struct StackMememberState {
    string message;
    string cardName;
    bool isCard;
};

struct MatchState {
    vector<PlayerBoardState> boards;
    vector<StackMememberState> stack;
    int currentI;
    int priorityI;

    int lootDeckCount;
    int treasureDeckCount;
    int monsterDeckCount;

    vector<string> lootDiscard;
    vector<string> treasureDiscard;
    vector<string> monsterDiscard;

    vector<string> shop;
    vector<string> monsters;
};

struct StackEffect {
    string funcName;
    Player* player;
    CardWrapper* card;
    string type;
    bool resolve = true;

    StackEffect(string funcName, Player* player, CardWrapper* card, string effect);
    StackEffect();
    StackMememberState getState();
};

class Match {
private:
    int _lastID = 0;
    bool _running = false;
    lua_State *L;
    int _turnCounter = 0;

    std::default_random_engine rng = std::default_random_engine {};
    std::vector<std::pair<CharacterCard*, bool>> _characterPool;

    std::vector<Player*> _players;
    int _currentI;
    int _nextI;
    int _priorityI;
    Player* _activePlayer;

    std::deque<LootCard*> _lootDeck;
    std::deque<LootCard*> _lootDiscard;

    std::deque<TrinketCard*> _treasureDeck;
    std::deque<TrinketCard*> _treasureDiscard;
    std::vector<TrinketCard*> _shop;
    int _lastTreasureIndex = -2;

    std::deque<MonsterCard*> _monsterDeck;
    std::deque<MonsterCard*> _monsterDiscard;
    std::vector<MonsterCard*> _monsters;

    std::vector<StackEffect> _stack;
    std::stack<string> _eotDefers;
    std::stack<StackEffect> _eotDeferredTriggers;

    // whenever a player plays a loot card that is a trinket, the original is placed in the pool
    // after a loot card trinket is destroyed, remove it's parent from _lootCardPool
    std::vector<LootCard*> _lootCardPool; 

    std::map<string, std::function<void(Player*, std::vector<string>)>> _actionMap = {
        {ACTION_PLAY_LOOT, [this](Player* player, std::vector<string> args){
            auto cardI = atoi(args[1].c_str());
            auto card = player->takeCard(cardI);
            auto wrapper = new CardWrapper(card, this->newCardID());
            this->pushToStack(StackEffect(
                "_popLootStack",
                player,
                wrapper,
                PLAY_LOOT_CARD_TYPE
            ));
            this->_lootStack.push(std::make_pair(card, player));
        }},
        {ACTION_BUY_TREASURE, [this](Player* player, std::vector<string> args){
            this->_lastTreasureIndex = atoi(args[1].c_str());
            this->pushToStack(StackEffect(
                "buyItem",
                player,
                nullptr,
                BUY_TREASURE_TYPE
            ));
        }}
    };

    std::stack<std::pair<LootCard*, Player*>> _lootStack;
public:
    Match();
    ~Match();
    void addToDiscard(LootCard* card);
    CardWrapper* cardWithID(int id);
    Player* findOwner(CardWrapper* card);
    void shuffleLootDiscardIntoMain();
    void shuffleTreasureDiscardIntoMain();
    void shuffleMonsterDiscardIntoMain();
    LootCard* getTopLootCard();
    TrinketCard* getTopTreasureCard();
    MonsterCard* getTopMonsterCard();
    vector<LootCard*> getTopLootCards(int amount);
    vector<TrinketCard*> getTopTreasureCards(int amount);
    vector<MonsterCard*> getTopMonsterCards(int amount);
    static int wrap_getOwner(lua_State *L);
    static int wrap_lootCards(lua_State *L);
    static int wrap_buyItem(lua_State* L);
    static int wrap_addCoins(lua_State *L);
    static int wrap_popLootStack(lua_State* L);
    static int wrap_deferEOT(lua_State *L);
    static int wrap_this(lua_State *L);
    static int wrap_incBeginningLoot(lua_State* L);
    static int wrap_decBeginningLoot(lua_State* L);
    static int wrap_plusOneTreasure(lua_State* L);
    static int wrap_incMaxLife(lua_State* L);
    static int wrap_decMaxLife(lua_State* L);
    static int wrap_getCardOwner(lua_State* L);
    static int wrap_getCurrentPlayer(lua_State* L);
    void addCardToBoard(TrinketCard* card, Player* owner);
    void removeFromShop(TrinketCard* card);
    Player* playerWithID(int id);
    void pushEOTDeferredTriggers();
    void execEOTDefers();
    void setupLua();
    static void lua_err(lua_State *L);
    void execScript(string script);
    void execFunc(string funcName);
    bool execCheck(string funcName, CardWrapper* card);
    void addToCharacterPool(CharacterCard* card);
    //  TODO remove the actions par
    Player* addPlayer(std::string name, CharacterCard* character, string actions);
    std::vector<CharacterCard*> getAvailableCharacters();
    CharacterCard* getRandomAvailableCharacter();
    void shuffleLootDeck();
    void shuffleTreasureDeck();
    void shuffleMonsterDeck();
    void createLootDeck(std::vector<std::pair<LootCard*, int>> pairs);
    void createTreasureDeck(std::vector<TrinketCard*> cards);
    void createMonsterDeck(std::vector<MonsterCard*> cards);
    void start();
    void passTurn();
    void calcNext();
    void currentPlayerLoot();
    void turn();
    void executePlayerAction(Player* player, string action);
    void applyTriggers(string triggerType);
    void pushToStack(const StackEffect& effect);
    void resolveStack();
    void resolveTop();
    string promptPlayerWithPriority();
    void log(string message);
    MatchState getState();
    int newCardID();
};