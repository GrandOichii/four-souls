//  TODO add costs to loot cards

#pragma once

#include <string>
#include <vector>
#include <map>

#include <iostream>

extern "C" {

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

#include "types.hpp"
#include "util.hpp"
#include "../util.hpp"

using std::string;

class Card {
protected:
    string _name;
    string _text;
    string _imagePath;
public:
    Card(string dir, json j);
    string name();
    string text();
    string imagePath();
    virtual void print(string prefix);
};

class ScriptCard : public Card {
private:
    string _script;
public:
    ScriptCard(string dir, json j);
    string script();
};

class TrinketCard : public ScriptCard {
private:
    bool _isEternal;

    std::map<string, std::pair<string, string>> _triggerMap;
    std::vector<ActivatedAbility> _abilities;

    string _enterFuncName = "";
    string _leaveFuncName = "";
public:
    TrinketCard(string dir, json j, bool isEternal);
    void print(string prefix) override;
    bool hasTrigger(string triggerName);
    std::pair<string, string> getTriggerWhen(string triggerName);
    string enterFuncName();
    string leaveFuncName();
    std::vector<ActivatedAbility> abilities();
};

class Player;
class Match;

class LootCard : public ScriptCard {
private:
    bool _isTrinket;
    TrinketCard* _trinket = nullptr;

    string _useFuncName;
    string _costFuncName;
    bool _goesToBottom;
public:
    LootCard(string dir, json j, bool isTrinket);
    ~LootCard();
    void print(string prefix) override;
    bool isTrinket();
    bool goesToBottom();
    string costFuncName();
    void use(Player* player, Match* match);
};

class CharacterCard : public Card {
private:
    int _attack;
    int _health;
    TrinketCard* _startingItem;
public:
    CharacterCard(string dir, json j);
    int attack();
    int health();
    TrinketCard* startingItem();
};

//  TODO change to inhereting ScriptCard
class MonsterCard : public Card {
private:

public:
    MonsterCard(string dir, json j);
};

class CardWrapper {
private:
    Card* _card;
    int _id;
    bool _tapped = false;
public:
    CardWrapper(Card* card, int id);
    Card* card();
    int id();
    void recharge();
    void tap();
    bool isActive();
    void pushTable(lua_State* L);
};