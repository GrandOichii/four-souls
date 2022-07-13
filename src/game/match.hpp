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

static const string CARD_INFO_FILE = "card.json";

const int MIN_PLAYER_COUNT = 2;
const int MAX_PLAYER_COUNT = 4;
const int SOULS_TO_WIN = 4;
const int STARTING_COIN_AMOUNT = 9;
const int STARTING_LOOT_AMOUNT = 2;
const int STARTING_SHOP_SIZE = 2;
const int STARTING_MONSTERS_AMOUNT = 2;
const int STARTING_TREASURE_PRICE = 10;
const int STARTING_ATTACK_COUNT = 1;

struct StackMememberState {
    string message;
    bool isCard;
    CardState card;
};

struct MatchState {
    vector<PlayerBoardState> boards;
    vector<StackMememberState> stack;
    int currentI;
    int priorityI;

    int lootDeckCount;
    int treasureDeckCount;
    int monsterDeckCount;

    vector<CardState> lootDiscard;
    vector<CardState> treasureDiscard;
    vector<string> monsterDiscard;

    vector<CardState> shop;
    vector<string> monsters; // turn monsters into card wrappers also
};

struct StackEffect {
    string funcName;
    Player* player;
    CardWrapper* cardW;
    string type;
    bool resolve = true;

    StackEffect(string funcName, Player* player, CardWrapper* card, string effect);
    StackEffect();
    StackMememberState getState();
};

struct DamageTrigger {
    string targetType;
    int id;
    int amount;
    int shelfLife;

    void pushTable(lua_State* L) {
        lua_newtable(L);
        l_pushtablestring(L, "type", this->targetType);
        l_pushtablenumber(L, "id", (float)this->id);
        l_pushtablenumber(L, "amount", (float)this->amount);
    }
};

template<class T>
static void millDeck(std::deque<T>& deck, std::deque<T>& discard, int amount) {
    while (amount) {
        if (deck.empty()) return;
        discard.push_back(deck.back());
        deck.pop_back();
        --amount;
    }
}

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

    std::deque<CardWrapper*> _lootDeck;
    std::deque<CardWrapper*> _lootDiscard;

    std::deque<CardWrapper*> _treasureDeck;
    std::deque<CardWrapper*> _treasureDiscard;
    std::vector<CardWrapper*> _shop;
    int _lastTreasureIndex = -2;

    std::deque<MonsterCard*> _monsterDeck;
    std::deque<MonsterCard*> _monsterDiscard;
    std::vector<MonsterCard*> _monsters;

    // StackEffect _lastStack;
    std::vector<StackEffect*> _stack;
    std::stack<string> _eotDefers;
    std::stack<StackEffect*> _eotDeferredTriggers;

    std::map<string, std::function<void(Player*, std::vector<string>)>> _actionMap = {
        {ACTION_PLAY_LOOT, [this](Player* player, std::vector<string> args){
            auto cardID = atoi(args[1].c_str());
            auto cardW = this->cardWithID(cardID);
            auto card = cardW->card();
            // request to pay cost
            auto cost = card->costFuncName();
            if (cost.size()) {
                bool payed = this->requestPayCost(cost, player);
                if (!payed) return;
            }
            player->takeCard(cardID);
            this->log(player->name() + " plays card " + card->name());
            this->pushToStack(new StackEffect(
                "_playTopLootCard",
                player,
                cardW,
                PLAY_LOOT_CARD_TYPE
            ));
            this->triggerLastEffectType();
        }},
        {ACTION_BUY_TREASURE, [this](Player* player, std::vector<string> args){
            this->_lastTreasureIndex = atoi(args[1].c_str());
            player->payPricePerTreasure();
            this->pushToStack(new StackEffect(
                "_buyItem",
                player,
                nullptr,
                BUY_TREASURE_TYPE
            ));
            this->triggerLastEffectType();
        }},
        {ACTION_ACTIVATE_CARD, [this](Player* player, std::vector<string> args){
            auto cardID = atoi(args[1].c_str());
            auto abilityI = atoi(args[2].c_str());
            auto w = this->cardWithID(cardID);
            auto card = w->card();
            auto ability = card->abilities()[abilityI];
            auto p = new StackEffect(
                ability.funcName,
                player,
                w,
                ACTIVATE_ITEM_TYPE
            );
            this->pushToStack(p);
            bool payed = this->requestPayCost(ability.costFuncName, player);
            if (!payed) {
                this->_stack.pop_back();
                delete p;
                return;
            }
            this->log(player->name() + " activated " + card->name());
            this->triggerLastEffectType();
        }}
    };

    std::map<string, std::function<void(int)>> _millMap = {
        {LOOT_DECK, [this](int amount){
            millDeck<CardWrapper*>(this->_lootDeck, this->_lootDiscard, amount);
        }},
        {TREASURE_DECK, [this](int amount){
            millDeck<CardWrapper*>(this->_treasureDeck, this->_treasureDiscard, amount);
        }},
        {MONSTER_DECK, [this](int amount){
            millDeck<MonsterCard*>(this->_monsterDeck, this->_monsterDiscard, amount);
        }}
    };

    // std::stack<std::pair<LootCard*, Player*>> _lootStack;
    std::stack<DamageTrigger> _damageStack;
    std::vector<std::pair<string, int>> _targetStack;

    std::vector<CardWrapper*> _allWrappers;
public:
    Match();
    ~Match();
    CardWrapper* addWrapper(ScriptCard* card);

    void addToLootDiscard(CardWrapper* card);
    CardWrapper* cardWithID(int id);
    Player* findOwner(CardWrapper* card);
    void shuffleLootDiscardIntoMain();
    void shuffleTreasureDiscardIntoMain();
    void shuffleMonsterDiscardIntoMain();
    CardWrapper* getTopLootCard();
    CardWrapper* getTopTreasureCard();
    MonsterCard* getTopMonsterCard();
    vector<CardWrapper*> getTopLootCards(int amount);
    vector<CardWrapper*> getTopTreasureCards(int amount);
    vector<MonsterCard*> getTopMonsterCards(int amount);
    bool requestPayCost(string costFuncName, Player* player);
    void triggerLastEffectType();
    static int wrap_addBlueHealth(lua_State* L);
    static int wrap_tapCard(lua_State* L);
    static int wrap_rechargeCard(lua_State* L);
    static int wrap_getDamageEvent(lua_State* L);
    static int wrap_popTarget(lua_State* L);
    static int wrap_pushTarget(lua_State* L);
    static int wrap_requestChoice(lua_State* L);
    static int wrap_getPlayers(lua_State* L);
    static int wrap_getOwner(lua_State *L);
    static int wrap_dealDamage(lua_State *L);
    static int wrap_playTopLootCard(lua_State *L);
    static int wrap_incAttackCount(lua_State *L);
    static int wrap_lootCards(lua_State *L);
    static int wrap_buyItem(lua_State* L);
    static int wrap_addCoins(lua_State *L);
    static int wrap_subCoins(lua_State *L);
    static int wrap_popLootStack(lua_State* L);
    static int wrap_deferEOT(lua_State *L);
    static int wrap_this(lua_State *L);
    static int wrap_incBeginningLoot(lua_State* L);
    static int wrap_decBeginningLoot(lua_State* L);
    static int wrap_plusOneTreasure(lua_State* L);
    static int wrap_incMaxLife(lua_State* L);
    static int wrap_tempIncMaxLife(lua_State* L);
    static int wrap_tempIncAttack(lua_State* L);
    static int wrap_decMaxLife(lua_State* L);
    // static int wrap_getCardOwner(lua_State* L);
    static int wrap_millDeck(lua_State* L);
    static int wrap_getCurrentPlayer(lua_State* L);
    static int wrap_addSouls(lua_State* L);
    static int wrap_setNextPlayer(lua_State* L);
    static int wrap_incTreasureCost(lua_State* L);
    static int wrap_decTreasureCost(lua_State* L);
    static int wrap_getTopOwner(lua_State* L);
    void resetEOT();
    void addCardToBoard(CardWrapper* card, Player* owner);
    void removeFromShop(CardWrapper* card);
    Player* playerWithID(int id);
    void pushEOTDeferredTriggers();
    void execEOTDefers();
    void setupLua(string setupScript);
    static void lua_err(lua_State *L);
    void execScript(string script);
    void execEnter(CardWrapper* w, Player* owner);
    void execFunc(string funcName);
    bool execCheck(string funcName, CardWrapper* card);
    void addToCharacterPool(CharacterCard* card);
    //  TODO remove the actions part
    Player* addPlayer(std::string name, CharacterCard* character, string actions, string responses);
    std::vector<CharacterCard*> getAvailableCharacters();
    CharacterCard* getRandomAvailableCharacter();
    void shuffleLootDeck();
    void shuffleTreasureDeck();
    void shuffleMonsterDeck();
    void createLootDeck(std::vector<std::pair<ScriptCard*, int>> pairs);
    void createTreasureDeck(std::vector<ScriptCard*> cards);
    void createMonsterDeck(std::vector<MonsterCard*> cards);
    void start();
    void passTurn();
    void calcNext();
    void currentPlayerLoot();
    void turn();
    void executePlayerAction(Player* player, string action);
    int applyTriggers(string triggerType);
    void pushToStack(StackEffect* effect);
    void resolveStack();
    void resolveTop();
    string promptPlayerWithPriority();
    void log(string message, bool wait=true);
    MatchState getState();
    int newCardID();
};