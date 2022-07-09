#pragma once

#include <string>
#include <vector>
#include <map>

#include <iostream>

#include "../util.hpp"
#include "types.hpp"

using std::string;

class Card {
protected:
    string _name;
    string _text;
    string _imagePath;
public:
    Card(string dir, json j) : 
        _name(j["name"]),
        _text(j["text"]),
        _imagePath(fs::join(dir, j["image"])) { /*std::cout << _name << std::endl;*/ }

    string name() { return _name; }
    string text() { return _text; }

    virtual void print(string prefix) {
        std::cout << prefix << _name << std::endl;
    }
};

class ScriptCard : public Card {
private:
    string _script;
public:
    ScriptCard(string dir, json j) : 
        Card(dir, j)
    { 
        this->_script = fs::readFile(fs::join(dir, j["script"]).c_str());
    }

    string script() { return _script; }
};

class TrinketCard : public ScriptCard {
private:
    bool _isEternal;

    std::map<string, std::pair<string, string>> _triggerMap;
    std::vector<ActivatedAbility> _abilities;

    string _enterFuncName = "";
    string _leaveFuncName = "";
public:
    TrinketCard(string dir, json j, bool isEternal) :
        ScriptCard(dir, j),
        _isEternal(isEternal)
    {
        std::cout << _name << std::endl;
        if (j.contains("on")) {
            auto triggers = j["on"];
            for (const auto& pair : triggers.items()) {
                auto v = pair.value();
                _triggerMap[pair.key()] = std::make_pair(
                    v["check"],
                    v["effect"]
                );
            }
        }
        if (j.contains("onEnter"))
            this->_enterFuncName = j["onEnter"];
        if (j.contains("onLeave"))
            this->_leaveFuncName = j["onLeave"];
        if (j.contains("abilities")) {
            for (const auto& jj : j["abilities"].items()) {
                auto a = jj.value();
                ActivatedAbility ability;
                ability.funcName = a["ability"];
                for (const auto& c : a["cost"].items())
                    ability.cost.push_back(c.value());
                _abilities.push_back(ability);
            }
        }
    }

    void print(string prefix) override {
        ScriptCard::print(prefix);
        std::cout << prefix << "Is eternal: " << _isEternal << std::endl;
        std::cout << prefix << "Enter script:" << std::endl << prefix << "\t" << _enterFuncName << std::endl;
        std::cout << prefix << "Leave script:" << std::endl << prefix << "\t" << _leaveFuncName << std::endl;
        std::cout << prefix << "trigger map:" << std::endl;
        for (const auto& [key, value] : _triggerMap) {
            std::cout << prefix << "\t" << key << " : " << value.second << std::endl;
        }
        std::cout << prefix << "Activations:" << std::endl;
        for (auto& ability : _abilities) {
            ability.print();
        }
    }

    bool hasTrigger(string triggerName) { return _triggerMap.count(triggerName); }
    std::pair<string, string> getTriggerWhen(string triggerName) { return _triggerMap[triggerName]; }
};

class LootCard : public ScriptCard {
private:
    bool _isTrinket;
    TrinketCard* _trinket = nullptr;

    string _useFuncName;
public:
    LootCard(string dir, json j, bool isTrinket) : 
        ScriptCard(dir, j),
        _isTrinket(isTrinket) 
    {
        if (!_isTrinket) {
            this->_useFuncName = j["use"];
            return;
        }
        this->_trinket = new TrinketCard(dir, j, false);
    }

    ~LootCard() {
        delete _trinket;
    }

    void print(string prefix) override {
        ScriptCard::print(prefix);
        if (_isTrinket) {
            std::cout << "Trinket:" << std::endl;
            this->_trinket->print("\t");
        } else {
            std::cout << "Use: " << std::endl << "\t" << _useFuncName << std::endl;;
        }
    }
};

class CharacterCard : public Card {
private:
    int _attack;
    int _health;
    TrinketCard* _startingItem;
    string _startingItemName;
public:
    CharacterCard(string dir, json j) : 
        Card(dir, j),
        _attack(j["attack"]),
        _health(j["health"]),
        _startingItemName(j["item"])
    {
        //  TODO add starting item
    }

    int attack() { return _attack; }
    int health() { return _health; }
    TrinketCard* startingItem() { return _startingItem; }
    string startingItemName() { return _startingItemName; }

    void setStartingItem(TrinketCard* card) {
        this->_startingItem = card;
    }

};

class CardWrapper {
private:
    Card* _card;
    int _id;
    bool _tapped;
public:
    CardWrapper(Card* card, int id) : 
        _card(card),
        _id(id) {}

    Card* card() { return _card; }
    int id() { return _id; }
    void recharge() { _tapped = false; }
    bool active() { return !_tapped; }
};