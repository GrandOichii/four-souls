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

class Player;

class ScriptCard : public Card {
private:
    string _script;
    bool _isEternal;

    std::map<string, std::pair<string, string>> _triggerMap;
    std::vector<ActivatedAbility> _abilities;

    string _enterFuncName = "";
    string _leaveFuncName = "";

    bool _isTrinket;
    string _useFuncName;
    string _costFuncName;
    bool _goesToBottom;
public:
    ScriptCard(string dir, json j, bool isTrinket, bool isEternal=false);
    ~ScriptCard();
    string script();
    bool hasTrigger(string triggerName);
    std::pair<string, string> getTriggerWhen(string triggerName);
    string useFuncName();
    string enterFuncName();
    string leaveFuncName();
    std::vector<ActivatedAbility> abilities();
    bool isTrinket();
    bool goesToBottom();
    string costFuncName();
};

class CharacterCard : public Card {
private:
    int _attack;
    int _health;
    ScriptCard* _startingItem;
public:
    CharacterCard(string dir, json j);
    int attack();
    int health();
    ScriptCard* startingItem();
};

//  TODO change to inhereting ScriptCard
class MonsterCard : public Card {
private:

public:
    MonsterCard(string dir, json j);
};

struct CardState {
    string cardName;
    bool active;
    int id;
    int counters;
};

class CardWrapper {
private:
    ScriptCard* _card;
    int _id;
    bool _tapped = false;
    Player* _owner;
    int _counters = 0;
public:
    CardWrapper(ScriptCard* card, int id);
    ScriptCard* card();
    int id();
    void recharge();
    void tap();
    bool isActive();
    void pushTable(lua_State* L);
    Player* owner();
    void setOwner(Player* owner);
    CardState getState();
    void addCounters(int amount);
    void removeCounters(int amount);
};