#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <thread>
#include <deque>
#include <map>
#include <stack>
#include <queue>
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

struct StackMemberState {
    string message;
    bool isCard;
    CardState card;
    string type;
    vector<int> targets;
};

struct RollEventState;

struct MatchState {
    vector<PlayerBoardState> boards;
    vector<StackMemberState> stack;
    vector<RollEventState> rollStack;
    int currentI;
    int priorityI;
    int currentID;
    bool isMain;
    int turnCounter;

    int lootDeckCount;
    int treasureDeckCount;
    int monsterDeckCount;

    vector<CardState> lootDiscard;
    vector<CardState> treasureDiscard;
    vector<CardState> monsterDiscard;

    vector<CardState> shop;
    vector<CardState> monsters;
    vector<MonsterDataState> monsterDataArr;
    bool isCombat;

    MatchState();
    MatchState(nlohmann::json j);
    
    void pushTable(lua_State* L) const;
    nlohmann::json toJson();
};

struct StackEffect {
    string funcName;
    Player* player = nullptr;
    CardWrapper* cardW = nullptr;
    string type;

    //  TODO maybe fix to pair<string, int>
    vector<int> targets;

    StackEffect(string funcName, Player* player, CardWrapper* card, string effect);
    StackEffect();
    void pushTable(lua_State* L);
    StackMemberState getState();
};

struct DamageTrigger {
    string sourceType;
    int sourceID;

    string targetType;
    int targetID;
    
    int amount;
    bool isCombatDamage;

    int shelfLife;

    void pushTable(lua_State* L) {
        lua_newtable(L);
        l_pushtablestring(L, "sourceType", this->sourceType);
        l_pushtablenumber(L, "sourceID", (float)this->sourceID);

        l_pushtablestring(L, "targetType", this->targetType);
        l_pushtablenumber(L, "targetID", (float)this->targetID);

        l_pushtablenumber(L, "amount", (float)this->amount);
        l_pushtableboolean(L, "isCombatDamage", isCombatDamage);
    }
};

struct RollEventState {
    int value;
    bool isCombatRoll;
    int ownerID;
};

struct RollEvent {
    int value;
    Player* owner;
    bool isCombatRoll;
    StackEffect* effect = nullptr;
    bool visible = true;

    RollEvent(Player* owner, bool isCombatRoll, StackEffect* effect = nullptr) :
        owner(owner),
        effect(effect),
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
    }

    RollEventState getState() {
        RollEventState result;
        result.value = value;
        result.isCombatRoll = isCombatRoll;
        result.ownerID = owner->id();
        return result;
    }
};

struct RewardEvent {
    CardWrapper* monsterW = nullptr;
};

struct DeathEvent {
    string type;
    int id;

    void pushTable(lua_State* L) {
        lua_newtable(L);
        l_pushtablestring(L, "type", type.c_str());
        l_pushtablenumber(L, "id", id);
    }
};

struct QueuedTrigger {
    Trigger effect;
    CardWrapper* cardW;
    Player* owner = nullptr;
};

class Match {
private:
    // config
    int _logWait;
    int _soulsToWin;
    int _startingLootAmount;
    int _startingCoinAmount;
    int _startingShopSize;
    int _startingMonstersAmount;
    int _startingTreasurePrice;
    int _startingAttackCount;
    int _startingPlayableCount;
    int _startingPurchaseCount;
    int _perDeathCoins;
    int _perDeathLoot;
    int _perDeathItems;
    int _maxHandSize;

    bool _turnEnd = false;
    int _lastID = 0;
    bool _running = false;
    lua_State *L;
    int _turnCounter = 0;

    std::default_random_engine rng = std::default_random_engine {};
    std::vector<std::pair<CharacterCard*, bool>> _characterPool;

    Player* _winner = nullptr;
    std::vector<Player*> _players;
    int _currentI;
    int _priorityI;
    bool _isMainPhase;
    Player* _activePlayer = nullptr;

    std::vector<RollEvent> _rollStack;
    int _lastRoll = -1;
    bool _lastRollIsCombat = false;
    int _lastRollOwnerID = -1;
    int _lastCRollValue = -1;

    std::vector<CardWrapper*> _bonusSouls;

    std::deque<CardWrapper*> _lootDeck;
    std::deque<CardWrapper*> _lootDiscard;

    std::deque<CardWrapper*> _treasureDeck;
    std::deque<CardWrapper*> _treasureDiscard;
    std::vector<CardWrapper*> _shop;
    int _lastTreasureIndex = -2;

    std::deque<CardWrapper*> _monsterDeck;
    std::deque<CardWrapper*> _monsterDiscard;
    std::vector<std::vector<CardWrapper*>> _monsters;
    std::vector<MonsterData*> _monsterDataArr;
    DamageTrigger _lastCombatDamageEvent;
    int _lastMonsterIndex = -2;
    bool _isAttackPhase = false;
    std::stack<RewardEvent> _rewardsStack;

    // StackEffect _lastStack;
    std::vector<StackEffect*> _stack;
    std::stack<StackEffect*> _eotDefers;
    std::stack<StackEffect*> _eotDeferredTriggers;

    std::map<string, std::function<void(Player*, std::vector<string>)>> _actionMap = {
        {ACTION_PASS, [this](Player* player, std::vector<string> args) {
            this->log(this->_activePlayer->name() + " tried to pass the turn, but is in combat");
        }},
        {ACTION_PLAY_LOOT, [this](Player* player, std::vector<string> args){
            auto cardID = std::stoi(args[1].c_str());
            auto cardW = this->cardWithID(cardID);
            auto card = cardW->card();
            Effect& effect = card->useEffect();
            effect.pushMe(this, cardW, player, PLAY_LOOT_CARD_TYPE);
            player->takeCard(cardID);
            this->log(player->name() + " plays card " + card->name());
            player->decPlayableAmount();
        }},
        {ACTION_BUY_TREASURE, [this](Player* player, std::vector<string> args){
            this->_lastTreasureIndex = std::stoi(args[1].c_str());
            player->decPurchaseAmount();
            this->pushToStack(new StackEffect(
                "_buyItem",
                player,
                nullptr,
                BUY_TREASURE_TYPE
            ));
            this->applyTriggers(BUY_TREASURE_TYPE);
        }},
        {ACTION_ACTIVATE_CARD, [this](Player* player, std::vector<string> args){
            auto cardID = std::stoi(args[1].c_str());
            auto abilityI = std::stoi(args[2].c_str());
            auto w = this->cardWithID(cardID);
            auto card = w->card();
            auto ability = card->abilities()[abilityI];
            ability.pushMe(this, w, player, ACTIVATE_ITEM_TYPE);
        }},
        {ACTION_ACTIVATE_CHARACTER_CARD, [this](Player* player, std::vector<string> args) {
            player->tapCharacter();
            auto cardW = player->characterCard();
            auto card = cardW->card();
            Effect& effect = card->useEffect();
            effect.pushMe(this, cardW, player, ACTIVATE_CHARACTER_TYPE);
            this->log(player->name() + " activates his characted card");
        }},
        {ACTION_ATTACK_MONSTER, [this](Player* player, std::vector<string> args) {
            this->_lastMonsterIndex = std::stoi(args[1].c_str());
            player->processAttackIndex(_lastMonsterIndex);
            if (this->_lastMonsterIndex != -1) this->_monsterDataArr[this->_lastMonsterIndex]->setIsBeingAttacked(true);
            this->pushToStack(new StackEffect(
                "_attackMonster",
                player,
                nullptr,
                ATTACK_MONSTER_TYPE
            ));
            this->triggerLastEffectType();
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

    int _lastKillerID = -1;

    DamageTrigger _lastDamageEvent;
    std::vector<DeathEvent> _deathStack;
    DeathEvent _lastDeath;
    std::vector<std::pair<string, int>> _targetStack;

    std::vector<CardWrapper*> _allWrappers;

    nlohmann::json _record;
    string _recordPath;
    int _seed = 0;
public:
    Match(nlohmann::json config);
    Match(nlohmann::json config, int seed);
    ~Match();
    void saveResponse(string playerName, string response);
    void saveResponse(string playerName, int response);
    void killPlayer(int id);
    void killMonster(CardWrapper* w);
    void updateAllPlayers(MatchState& state);
    void updateAllPlayersEndMatch();
    CardWrapper* addWrapper(ScriptCard* card);
    void sortToDiscard(CardWrapper* card);
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
    Player* getCurrentPlayer();
    bool requestPayCost(string costFuncName, Player* player);
    int dealDamage(string tgtType, int tgtID, int amount);
    void triggerLastEffectType();
    void pushPlayers(lua_State* L);
    void pushDamageEvent(DamageTrigger event);
    void refillDeadMonsters();
    void queueTrigger(CardWrapper* wrapper, string triggerType, Player* owner, std::queue<QueuedTrigger>& out);
    void pushEffect(string funcName, Effect& effect, CardWrapper* cardW, Player* owner, string type);
    //  TODO add wrap_popBonusCards, inside of it call refillDeadMonsters
    static int wrap_canFlip(lua_State* L);
    static int wrap_setCurrentPlayer(lua_State* L);
    static int wrap_placeOnTop(lua_State* L);
    static int wrap_cardWithID(lua_State* L);
    static int wrap_addAttackOpportunity(lua_State* L);
    static int wrap_setIsEternal(lua_State* L);
    static int wrap_pushToStack(lua_State* L);
    static int wrap_getMonsterPiles(lua_State* L);
    static int wrap_pushRefillShop(lua_State* L);
    static int wrap_pushRefillMonsters(lua_State* L);
    static int wrap_refillMonsters(lua_State* L);
    static int wrap_getShop(lua_State* L);
    static int wrap_refillShop(lua_State* L);
    static int wrap_incSkipCounter(lua_State* L);
    static int wrap_flip(lua_State* L);
    static int wrap_tapCharacterCard(lua_State* L);
    static int wrap_getCharacterCard(lua_State* L);
    static int wrap_getSoulsOf(lua_State* L);
    static int wrap_destroySoul(lua_State* L);
    static int wrap_moveToHand(lua_State* L);
    static int wrap_moveToBoard(lua_State* L);
    static int wrap_removeFromEverywhere(lua_State* L);
    static int wrap_dealCombatDamageF(lua_State* L);
    static int wrap_incPurchaseAmount(lua_State* L);
    static int wrap_cancelCurrentAttack(lua_State* L);
    static int wrap_setTurnEnd(lua_State* L);
    static int wrap_getMRoll(lua_State* L);
    static int wrap_getMPower(lua_State* L);
    static int wrap_getTurnCounter(lua_State* L);
    static int wrap_getMaxHealth(lua_State* L);
    static int wrap_getAttack(lua_State* L);
    static int wrap_getLastKillerID(lua_State* L);
    static int wrap_popDeathStack(lua_State* L);
    static int wrap_getLastDeath(lua_State* L);
    static int wrap_incAdditionalCoins(lua_State* L);
    static int wrap_decAdditionalCoins(lua_State* L);
    static int wrap_getRollStack(lua_State* L);
    static int wrap_setRollValue(lua_State* L);
    static int wrap_addBlueHealth(lua_State* L);
    static int wrap_activateRoll(lua_State* L);
    static int wrap_getLastRoll(lua_State* L);
    static int wrap_popRollStack(lua_State* L);
    static int wrap_putFromTopToBottom(lua_State* L);
    static int wrap_millDeck(lua_State* L);
    static int wrap_addPlayableCount(lua_State* L);
    static int wrap_resetCounters(lua_State* L);
    static int wrap_tapCard(lua_State* L);
    static int wrap_rechargeCard(lua_State* L);
    static int wrap_getStack(lua_State* L);
    static int wrap_expandActiveMonsters(lua_State* L);
    static int wrap_getTopDamageEvent(lua_State* L);
    static int wrap_getDamageEvent(lua_State* L);
    static int wrap_destroyCard(lua_State* L);
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
    static int wrap_getActiveMonsters(lua_State* L);
    static int wrap_getDeathStack(lua_State* L);
    static int wrap_dealCombatDamage(lua_State *L);
    static int wrap_lootCards(lua_State *L);
    static int wrap_buyItem(lua_State* L);
    static int wrap_addCoins(lua_State *L);
    static int wrap_subCoins(lua_State *L);
    static int wrap_popLootStack(lua_State* L);
    static int wrap_deferEOT(lua_State *L);
    static int wrap_this(lua_State *L);
    static int wrap_rechargeActivePlayer(lua_State *L);
    static int wrap_healPlayer(lua_State *L);
    static int wrap_healMonster(lua_State *L);
    static int wrap_placeInMonsterPile(lua_State *L);
    static int wrap_incBeginningLoot(lua_State* L);
    static int wrap_decBeginningLoot(lua_State* L);
    static int wrap_discardLoot(lua_State* L);
    static int wrap_discardMe(lua_State* L);
    static int wrap_gainTreasure(lua_State* L);
    static int wrap_killEntity(lua_State* L);
    static int wrap_topCardsOf(lua_State* L);
    static int wrap_getCurrentPlayer(lua_State* L);
    static int wrap_addSoulCard(lua_State* L);
    static int wrap_getActivations(lua_State* L);
    static int wrap_getMonsterHealth(lua_State* L);
    static int wrap_incTreasureCost(lua_State* L);
    static int wrap_decTreasureCost(lua_State* L);
    static int wrap_getTopOwner(lua_State* L);
    static int wrap_attackMonster(lua_State* L);
    static int wrap_expandShopSize(lua_State* L);
    static int wrap_addCurse(lua_State* L);
    static int wrap_getDiscard(lua_State* L);
    static int wrap_rechargeCharacterCard(lua_State* L);
    void healMonsters();
    void rollAttack();
    void resetEOT();
    void addCardToBoard(CardWrapper* card, Player* owner);
    void removeFromShop(CardWrapper* card);
    Player* playerWithID(int id);
    void pushEOTDeferredTriggers();
    void execEOTDefers();
    void setupLua(string setupScript);
    static void lua_err(lua_State *L);
    void execScript(string script);
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
    void createMonsterDeck(std::vector<MonsterCard*> cards, std::vector<ScriptCard*> bCards);
    void createBonusSouls(std::vector<ScriptCard*> cards);
    void start();
    void passTurn();
    void currentPlayerLoot();
    void turn();
    void executePlayerAction(Player* player, string action);
    void applyTriggers(string triggerType);
    void pushToStack(StackEffect* effect);
    void resolveStack();
    void resolveTop();
    string promptPlayerWithPriority();
    void log(string message, bool wait=true);
    MatchState getState();
    int newCardID();
    void pushDeathEvent(string type, int id);
    void dumpStacks();

    friend StackEffect* Effect::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type);
    friend StackEffect* Trigger::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type);
};