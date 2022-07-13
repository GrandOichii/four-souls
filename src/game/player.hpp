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
    
    std::pair<string, bool> playerCard;
    std::vector<CardState> board;
    std::vector<CardState> hand;
};

class Player {
private:
    CharacterCard* _characterCard;
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
public:
    Player(std::string name, CharacterCard* card, int id);
    virtual ~Player();
    void print();
    CardWrapper* takeCard(int cardID);
    std::vector<CardWrapper*> board();
    std::vector<CardWrapper*> hand();
    bool characterActive();
    void rechargeCharacter();
    void rechargeCards();
    void recharge();
    void pushTable(lua_State* L);

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
    void dealDamage(int amount);

    virtual string promptAction() = 0;
    virtual string promptResponse(string text, string choiceType, vector<int> choices) = 0;

    string name();
    int id();
    int soulCount();
    CharacterCard* characterCard();

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

class ScriptedPlayer : public Player {
private:
    std::stack<string> _actions;
    std::stack<string> _responses;
public:
    ScriptedPlayer(std::string name, CharacterCard* card, int id, string actions, string responses);

    string promptAction();
    string promptResponse(string text, string choiceType, vector<int> choices);
};
