#pragma once

#include <string>
#include <vector>
#include <stack>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

#include "cards.hpp"

using std::string;

struct PlayerBoardState {
    int coinCount;
    bool characterActive;
    int health;
    int maxHealth;
    int blueHealth;
    int attack;
    int playableCount;
    int purchaseCount;
    int attackCount;
    int treasurePrice;
    bool isDead;
    int id;
    string name; 

    CardState playerCard;
    std::vector<CardState> board;
    std::vector<CardState> hand;
    std::vector<CardState> souls;

    void pushTable(lua_State* L) const;
};

// string toJson(const PlayerBoardState& state);

struct MatchState;
class Match;

class Player {
private:
    Match* _parent;
    lua_State* L = nullptr;
    CardWrapper* _characterCard;
    CharacterCard* _characterCardOrig;
    string _name;
    bool _isDead = false;

    int _health;
    int _blueHealth;

    int _maxAttackCount;
    int _attackCount;

    int _id;

    int _coinCount;
    int _additionalCoins;

    bool _characterActive;
    int _startTurnLootAmount;

    int _treasurePrice;

    std::vector<CardWrapper*> _board;
    std::vector<CardWrapper*> _hand;
    std::vector<CardWrapper*> _souls;

    int _playableCount;
    int _maxPlayableCount;

    int _purchaseCount;
    int _maxPurchaseCount;

public:
    Player(std::string name, CharacterCard* card, int id);
    virtual ~Player();
    void setParent(Match* parent);
    void setLuaENV(lua_State* L);
    void print();
    CharacterCard* origCharacterCard();;
    CardWrapper* takeCard(int cardID);
    std::vector<CardWrapper*> board();
    std::vector<CardWrapper*> hand();
    std::vector<CardWrapper*> souls();
    bool removeCard(CardWrapper* card);
    void tapCharacter();
    bool characterActive();
    void rechargeCharacter();
    void rechargeCards();
    void recharge();
    void pushTable(lua_State* L);

    void resetPlayableCount();
    int getPlayableAmount();
    void decPlayableAmount();
    void incPlayableAmount();
    void setPlayableCount(int amount);

    void resetPurchaseCount();
    void setPurchaseCount(int amount);
    void decPurchaseAmount();
    void incPurchaseAmount();

    void decMonsterAttackAmount();

    void incMaxAttackCount();
    void decMaxAttackCount();
    void incAttackCount();

    bool isDead();
    void setIsDead(bool value);

    int attack();
    int baseAttack();

    void addBlueHealth(int amount);
    int blueHealth();

    void incBeginningLoot();
    void decBeginningLoot();

    int health();
    void heal(int amount);
    int maxHealth();
    int baseMaxHealth();
    int dealDamage(int amount);

    virtual void update(MatchState& state) = 0;
    virtual void updateEndMatch(MatchState& state, int winnerID) = 0;
    virtual string promptAction(MatchState& state) = 0;
    virtual string promptResponse(MatchState& state, string text, string choiceType, vector<int> choices) = 0;
    virtual string promptSimpleResponse(MatchState& state, string text, vector<string> choices) = 0;
    virtual string promptChooseCardsInHand(MatchState& state, string text, int targetID, int amount) = 0;

    string name();
    int id();
    int soulCount();
    CardWrapper* characterCard();

    int coinCount();
    void addCoins(int amount);
    void removeCoins(int amount);
    void payPricePerTreasure();

    void incAdditionalCoins();
    void decAdditionalCoins();

    void incTreasureCost(int amount);
    void decTreasureCost(int amount);

    void addLootCards(vector<CardWrapper*> cards);

    PlayerBoardState getState();
    void addToBoard(CardWrapper* w);
    void removeFromBoard(CardWrapper* w);

    void addSoulCard(CardWrapper* card);
    CardWrapper* removeSoulCard(int cid);

    void resetEOT();
    void setStartingValues(int treasurePrice, int attackCount, int playableCount, int purchaseCount);
};

class BotPlayer : public Player {
private:
    lua_State* L;
public:
    BotPlayer(std::string name, CharacterCard* card, int id, string script);
    ~BotPlayer();

    void update(MatchState& state);
    void updateEndMatch(MatchState& state, int winnerID);
    string promptAction(MatchState& state);
    string promptResponse(MatchState& state, string text, string choiceType, vector<int> choices);
    string promptSimpleResponse(MatchState& state, string text, vector<string> choices);
    string promptChooseCardsInHand(MatchState& state, string text, int targetID, int amount);
};