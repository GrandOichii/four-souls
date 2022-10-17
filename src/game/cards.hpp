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
class Match;

class Card {
protected:
    string _name;
    string _text;
    string _key;
    int _soulCount = 0;
    CardTypes _type;
public:
    Card(string dir, json j, CardTypes type);
    string name();
    string text();
    string key();
    int soulCount();
    CardTypes type();
    virtual void print(string prefix);
};

class Player;

class ScriptCard : public Card {
private:
    string _script;
    bool _isEternal = false;

    std::map<string, Trigger> _triggerMap;
    std::vector<Effect> _abilities;

    std::vector<Effect> _custom;

    Effect _enterEffect;
    Effect _leaveEffect;
    Effect _destroyedEffect;

    Effect _useEffect;
    bool _goesToBottom = false;

    ScriptCard* _alt = nullptr;
public:
    ScriptCard(string dir, json j, CardTypes type);
    ~ScriptCard();
    string script();
    bool hasTrigger(string triggerName);
    Trigger& getTriggerWhen(string triggerName);
    Effect& useEffect();
    Effect& enterEffect();
    Effect& leaveEffect();
    Effect& destroyedEffect();
    std::vector<Effect>& abilities();
    bool isEternal();
    bool hasAlt();
    ScriptCard* alt();
};

class CharacterCard : public ScriptCard {
private:
    int _attack;
    int _health;
    ScriptCard* _startingItem = nullptr;

    Effect _gameStartEffect;
public:
    CharacterCard(string dir, json j);
    ~CharacterCard();
    int attack();
    int health();
    ScriptCard* startingItem();

    Effect& gameStartEffect();
};

struct MonsterDataState {
    int health;
    int roll;
    int power;
    int blueHealth;
    bool canBeAttacked;
    void pushTable(lua_State* L) const;
};


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
    void heal(int amount);
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

    Trigger _deathEffect;
    Effect _rewardsEffect;

    MonsterData* _data = nullptr;
public:
    MonsterCard(string dir, json j);
    ~MonsterCard();
    Effect& rewardsEffect();
    Trigger& deathEffect();
    MonsterData* data();
    void deleteData();
    void createData(lua_State* L, Match* parent, int id);
};

struct CardState {
    string name;
    string key;
    bool active;
    int id;
    int counters;
    int soulCount;
    Zones zone;

    int ownerID;
    vector<EffectState> activatedAbilities;

    void pushTable(lua_State* L) const;
};

CardState emptyCardState();

class CardWrapper {
private:
    ScriptCard* _card;
    int _id;
    bool _tapped = false;
    Player* _owner = nullptr;
    int _counters = 0;
    bool _showAlt = false;
    bool _isEternal = false;
public:
    CardWrapper(ScriptCard* card, int id);
    bool isEternal();
    void setIsEternal(bool value);
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
    void flip();
    bool hasAlt();
};
