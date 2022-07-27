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

struct Trigger {
    string checkFuncName;
    string costFuncName = "";
    string effectFuncName;
    bool usesStack = true;

    // Trigger(string checkFuncName, string effectFuncName) :
    //     checkFuncName(checkFuncName),
    //     effectFuncName(effectFuncName) {}
};

class Card {
protected:
    string _name;
    string _text;
public:
    Card(string dir, json j);
    string name();
    string text();
    virtual void print(string prefix);
};

class Player;

class ScriptCard : public Card {
private:
    string _script;
    bool _isEternal;

    std::map<string, Trigger> _triggerMap;
    std::vector<ActivatedAbility> _abilities;

    string _enterFuncName = "";
    string _leaveFuncName = "";

    bool _isTrinket;
    string _useFuncName;
    string _costFuncName;
    bool _goesToBottom = false;
public:
    ScriptCard(string dir, json j, bool isTrinket, bool isEternal=false);
    ~ScriptCard();
    string script();
    bool hasTrigger(string triggerName);
    Trigger getTriggerWhen(string triggerName);
    string useFuncName();
    string enterFuncName();
    string leaveFuncName();
    std::vector<ActivatedAbility> abilities();
    bool isTrinket();
    bool isEternal();
    bool goesToBottom();
    string costFuncName();
};

class CharacterCard : public ScriptCard {
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

struct MonsterDataState {
    int health;
    int roll;
    int power;
    int blueHealth;
};

class MonsterData {
private:
    int _basePower;
    int _baseRoll;
    int _health;
    int _blueHealth;
    int _maxHealth;
    bool _isBeingAttacked = false;
public:
    MonsterData(int health, int roll, int power);
    MonsterDataState getState();
    int roll();
    int health();
    int power();
    int dealDamage(int amount);
    void fullHeal();
    int maxHealth();
    bool isBeingAttacked();
    void setIsBeingAttacked(bool value);
    void nullHealth();
};

class MonsterCard : public ScriptCard {
private:
    int _baseHealth;
    int _basePower;
    int _baseRoll;

    MonsterData* _data = nullptr;
    string _rewardsFuncName;
public:
    MonsterCard(string dir, json j);
    ~MonsterCard();
    string rewardsFuncName();
    MonsterData* data();
    void resetData();
};

struct CardState {
    string cardName;
    bool active;
    int id;
    int counters;
    Zones zone;

    int ownerID;
    int activatedAbilityCount;
};

class CardWrapper {
private:
    ScriptCard* _card;
    int _id;
    bool _tapped = false;
    Player* _owner = nullptr;
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
