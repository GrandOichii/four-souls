#pragma once

#include <string>
#include <vector>
#include <stack>

#include "cards.hpp"

using std::string;

struct PlayerBoardState {
    int coinCount;
    bool characterActive;
    int health;
    int maxHealth;
    int blueHealth;
    int soulCount;
    int attack;
    
    CardState playerCard;
    std::vector<CardState> board;
    std::vector<CardState> hand;

    void pushTable(lua_State* L);
};

struct MatchState;

class Player {
private:
    CardWrapper* _characterCard;
    CharacterCard* _characterCardOrig;
    std::string _name;

    int _maxHealth;
    int _health;
    int _blueHealth;
    int _tempMaxHealthBoost = 0;

    int _attack;
    int _tempAttackBoost = 0;

    int _maxAttackCount;
    int _attackCount;

    int _id;

    int _soulCount;

    int _coinCount;
    int _additionalCoins;

    bool _characterActive;
    int _startTurnLootAmount;

    int _treasurePrice;

    std::vector<CardWrapper*> _board;
    std::vector<CardWrapper*> _hand;

    int _playableCount;
    int _maxPlayableCount;

    int _purchaseCount;
    int _maxPurchaseCount;

public:
    Player(std::string name, CharacterCard* card, int id);
    virtual ~Player();
    void print();
    CharacterCard* origCharacterCard();;
    CardWrapper* takeCard(int cardID);
    std::vector<CardWrapper*> board();
    std::vector<CardWrapper*> hand();
    void tapCharacter();
    bool characterActive();
    void rechargeCharacter();
    void rechargeCards();
    void recharge();
    void pushTable(lua_State* L);

    void resetPlayableCount();
    void decPlayableAmount();
    void incPlayableAmount();
    void setPlayableCount(int amount);

    void resetPurchaseCount();
    void setPurchaseCount(int amount);
    void decPurchaseAmount();
    void incPurchaseAmount();

    void incMaxAttackCount();
    void decMaxAttackCount();
    void incAttackCount();

    int attack();
    void tempIncAttack(int amount);

    void addBlueHealth(int amount);
    int blueHealth();

    void incBeginningLoot();
    void decBeginningLoot();

    int health();
    int maxHealth();
    int dealDamage(int amount);

    virtual string promptAction(const MatchState& state) = 0;
    virtual string promptResponse(const MatchState& state, string text, string choiceType, vector<int> choices) = 0;
    virtual string promptSimpleResponse(const MatchState& state, string text, vector<string> choices) = 0;

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

    void incMaxLife(int amount);
    void tempIncMaxLife(int amount);
    void decMaxLife(int amount);

    void addSouls(int amount);

    void resetEOT();
};

class BotPlayer : public Player {
private:
    lua_State* L;
public:
    BotPlayer(std::string name, CharacterCard* card, int id, string script);
    ~BotPlayer();

    string promptAction(const MatchState& state);
    string promptResponse(const MatchState& state, string text, string choiceType, vector<int> choices);
    string promptSimpleResponse(const MatchState& state, string text, vector<string> choices);
};