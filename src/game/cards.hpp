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
    int _soulCount = 0;
    CardTypes _type;
public:
    Card(string dir, json j, CardTypes type);
    string name();
    string text();
    int soulCount();
    CardTypes type();
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
    ScriptCard(string dir, json j, CardTypes type, bool isTrinket, bool isEternal=false);
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
    bool canBeAttacked;
};

class Match;

class MonsterData {
private:
    lua_State* _L = nullptr;
    Match* _parent = nullptr;
    int _mid;
    int _basePower;
    int _baseRoll;
    int _health;
    int _blueHealth;
    int _maxHealth;
    bool _canBeAttacked = true;
    bool _isBeingAttacked = false;
public:
    MonsterData(lua_State* L, Match* parent, int mid, int health, int roll, int power, bool canBeAttacked);
    MonsterDataState getState();
    int roll();
    int baseRoll();
    int health();
    int baseHealth();
    int power();
    int basePower();
    int dealDamage(int amount);
    void fullHeal();
    int maxHealth();
    bool isBeingAttacked();
    void setIsBeingAttacked(bool value);
    void nullHealth();
    bool canBeAttacked();
};

MonsterDataState emptyMonsterDataState();

class MonsterCard : public ScriptCard {
private:
    int _baseHealth;
    int _basePower;
    int _baseRoll;
    bool _canBeAttacked = true;

    string _deathFuncName = "";

    MonsterData* _data = nullptr;
    string _rewardsFuncName;
public:
    MonsterCard(string dir, json j);
    ~MonsterCard();
    string rewardsFuncName();
    string deathFuncName();
    MonsterData* data();
    void deleteData();
    void createData(lua_State* L, Match* parent, int id);
};

struct CardState {
    string cardName;
    bool active;
    int id;
    int counters;
    int soulCount;
    Zones zone;

    int ownerID;
    int activatedAbilityCount;
};

CardState emptyCardState();

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
    void resetCounters();
};
