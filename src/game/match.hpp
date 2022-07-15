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
const int STARTING_COIN_AMOUNT = 10;
const int STARTING_LOOT_AMOUNT = 3;
const int STARTING_SHOP_SIZE = 2;
const int STARTING_MONSTERS_AMOUNT = 2;
const int STARTING_TREASURE_PRICE = 10;
const int STARTING_ATTACK_COUNT = 1;
const int STARTING_PLAYABLE_COUNT = 1;
const int STARTING_PURCHASE_COUNT = 1;

struct StackMemberState {
    string message;
    bool isCard;
    CardState card;
};

struct MatchState {
    vector<PlayerBoardState> boards;
    vector<StackMemberState> stack;
    int currentI;
    int priorityI;
    int currentID;
    bool isMain;

    int lootDeckCount;
    int treasureDeckCount;
    int monsterDeckCount;

    vector<CardState> lootDiscard;
    vector<CardState> treasureDiscard;
    vector<CardState> monsterDiscard;

    vector<CardState> shop;
    vector<CardState> monsters; // turn monsters into card wrappers also

    void pushTable(lua_State* L) const;
    
    string toJson() const;

    MatchState();
    MatchState(string js);
};

struct StackEffect {
    string funcName;
    Player* player;
    CardWrapper* cardW;
    string type;
    bool resolve = true;

    StackEffect(string funcName, Player* player, CardWrapper* card, string effect);
    StackEffect();
    StackMemberState getState();
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

struct RollEvent {
    int value;
    Player* owner;
    bool isCombatRoll;

    RollEvent(Player* owner, bool isCombatRoll) :
        owner(owner),
        isCombatRoll(isCombatRoll) 
    {
        value = rand() % 6 + 1;
        // value = 1;
        // value = 2;
        // value = 3;
        // value = 4;
        // value = 5;
        // value = 6;
    }

    void pushTable(lua_State *L) {
        lua_newtable(L);
        l_pushtablenumber(L, "value", value);
        l_pushtablenumber(L, "ownerID", owner->id());
        l_pushtableboolean(L, "isCombatRoll", isCombatRoll);
        std::cout << "TABLE PUSHED" <<std::endl;
    }
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
    bool _isMainPhase;
    Player* _activePlayer;

    int _lastRoll = -1;
    int _lastRollOwnerID = -1;
    std::vector<RollEvent> _rollStack;

    std::deque<CardWrapper*> _lootDeck;
    std::deque<CardWrapper*> _lootDiscard;

    std::deque<CardWrapper*> _treasureDeck;
    std::deque<CardWrapper*> _treasureDiscard;
    std::vector<CardWrapper*> _shop;
    int _lastTreasureIndex = -2;

    std::deque<CardWrapper*> _monsterDeck;
    std::deque<CardWrapper*> _monsterDiscard;
    std::vector<CardWrapper*> _monsters;

    // StackEffect _lastStack;
    std::vector<StackEffect*> _stack;
    std::stack<string> _eotDefers;
    std::stack<StackEffect*> _eotDeferredTriggers;

    std::map<string, std::function<void(Player*, std::vector<string>)>> _actionMap = {
        {ACTION_PLAY_LOOT, [this](Player* player, std::vector<string> args){
            auto cardID = std::stoi(args[1].c_str());
            auto cardW = this->cardWithID(cardID);
            auto card = cardW->card();
            auto e = new StackEffect(
                "_playTopLootCard",
                player,
                cardW,
                PLAY_LOOT_CARD_TYPE
            );
            this->pushToStack(e);
            auto cost = card->costFuncName();
            if (cost.size()) {
                bool payed = this->requestPayCost(cost, player);
                if (!payed) {
                    this->_stack.pop_back();
                    delete e;
                    return;
                };
            }
            player->takeCard(cardID);
            this->log(player->name() + " plays card " + card->name());
            player->decPlayableAmount();
            
            this->triggerLastEffectType();
        }},
        {ACTION_BUY_TREASURE, [this](Player* player, std::vector<string> args){
            this->_lastTreasureIndex = std::stoi(args[1].c_str());
            player->payPricePerTreasure();
            player->decPurchaseAmount();
            this->pushToStack(new StackEffect(
                "_buyItem",
                player,
                nullptr,
                BUY_TREASURE_TYPE
            ));

            this->triggerLastEffectType();
        }},
        {ACTION_ACTIVATE_CARD, [this](Player* player, std::vector<string> args){
            auto cardID = std::stoi(args[1].c_str());
            auto abilityI = std::stoi(args[2].c_str());
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
        }},
        {ACTION_ACTIVATE_CHARACTER_CARD, [this](Player* player, std::vector<string> args) {
            player->tapCharacter();
            auto cardW = player->characterCard();
            auto p = new StackEffect(
                cardW->card()->useFuncName(),
                player,
                cardW,
                ACTIVATE_CHARACTER_TYPE
            );
            this->pushToStack(p);
            this->log(player->name() + " activates his characted card");
            this->triggerLastEffectType();
            // std::cout << "PLAYING " << ->card()->useFuncName() << std::endl;
        }}
    };

    std::map<string, std::deque<CardWrapper*>*> _deckMap = {
        {LOOT_DECK, &_lootDeck},
        {TREASURE_DECK, &_treasureDeck},
        {MONSTER_DECK, &_monsterDeck},
    };
    std::map<string, std::deque<CardWrapper*>*> _discardMap = {
        {LOOT_DECK, &_lootDiscard},
        {TREASURE_DECK, &_treasureDiscard},
        {MONSTER_DECK, &_monsterDiscard},
    };

    std::stack<DamageTrigger> _damageStack;
    std::vector<std::pair<string, int>> _targetStack;

    std::vector<CardWrapper*> _allWrappers;
public:
    Match();
    ~Match();
    void updateAllPlayers();
    CardWrapper* addWrapper(ScriptCard* card);
    void addToLootDiscard(CardWrapper* card);
    CardWrapper* cardWithID(int id);
    Player* findOwner(CardWrapper* card);
    void shuffleLootDiscardIntoMain();
    void shuffleTreasureDiscardIntoMain();
    void shuffleMonsterDiscardIntoMain();
    CardWrapper* getTopLootCard();
    CardWrapper* getTopTreasureCard();
    CardWrapper* getTopMonsterCard();
    vector<CardWrapper*> getTopLootCards(int amount);
    vector<CardWrapper*> getTopTreasureCards(int amount);
    vector<CardWrapper*> getTopMonsterCards(int amount);
    bool requestPayCost(string costFuncName, Player* player);
    void triggerLastEffectType();
    void pushPlayers(lua_State* L);
    static int wrap_incAdditionalCoins(lua_State* L);
    static int wrap_decAdditionalCoins(lua_State* L);
    static int wrap_getRollStack(lua_State* L);
    static int wrap_setRollValue(lua_State* L);
    static int wrap_addBlueHealth(lua_State* L);
    static int wrap_pushRollEvent(lua_State* L);
    static int wrap_getLastRoll(lua_State* L);
    static int wrap_popRollStack(lua_State* L);
    static int wrap_putFromTopToBottom(lua_State* L);
    static int wrap_millDeck(lua_State* L);
    static int wrap_addPlayableCount(lua_State* L);
    static int wrap_tapCard(lua_State* L);
    static int wrap_rechargeCard(lua_State* L);
    static int wrap_getDamageEvent(lua_State* L);
    static int wrap_popTarget(lua_State* L);
    static int wrap_pushTarget(lua_State* L);
    static int wrap_addCounters(lua_State* L);
    static int wrap_removeCounters(lua_State* L);
    static int wrap_requestChoice(lua_State* L);
    static int wrap_requestCardsInHand(lua_State* L);
    static int wrap_requestSimpleChoice(lua_State* L);
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
    static int wrap_topCardsOf(lua_State* L);
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
    void addPlayer(Player* player);
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