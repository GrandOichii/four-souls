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
    int soulCount;
    
    std::pair<string, bool> playerCard;
    std::vector<std::pair<string, bool>> board;
    std::vector<string> hand;
};

class Player {
private:
    CharacterCard* _characterCard;
    std::string _name;

    int _maxHealth;
    int _health;

    int _baseAttack;
    int _id;

    int _soulCount;

    int _coinCount;
    int _additionalCoins;

    bool _characterActive;
    int _startTurnLootAmount;

    int _treasurePrice;

    std::vector<CardWrapper*> _board; // dont forget to delete these!
    std::vector<LootCard*> _hand;
public:
    Player(std::string name, CharacterCard* card, int id);
    virtual ~Player();
    void print();
    LootCard* takeCard(int cardI);
    std::vector<CardWrapper*> board();
    std::vector<LootCard*> hand();
    bool characterActive();
    void rechargeCharacter();
    void rechargeCards();
    void recharge();
    void pushTable(lua_State* L);

    void incBeginningLoot();
    void decBeginningLoot();

    int health();
    int maxHealth();

    virtual string promptAction() = 0;

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

    void addLootCards(vector<LootCard*> cards);

    PlayerBoardState getState();
    void addToBoard(CardWrapper* w);

    void incMaxLife(int amount);
    void decMaxLife(int amount);

    void addSouls(int amount);
};

class ScriptedPlayer : public Player {
private:
    std::stack<string> _actions;
public:
    ScriptedPlayer(std::string name, CharacterCard* card, int id, string actions);

    string promptAction();
};
