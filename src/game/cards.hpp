#pragma once

#include <string>
#include <vector>
#include <map>

#include <iostream>

#include "../util.hpp"

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
        _imagePath(fs::join(dir, j["image"])) {}

    string name() { return _name; }
    string text() { return _text; }
};

struct ActivatedAbility {
    std::vector<string> cost;
    string script;

    void print() {
        std::cout << "Cost: " << str::join(cost.begin(), cost.end(), ", ") << std::endl;
        std::cout << "Script: " << script << std::endl;
    }
};

class TrinketCard : public Card {
private:
    bool _isEternal;

    std::map<string, std::vector<string>> _eventMap;
    std::vector<ActivatedAbility> _abilities;

    string _enterScript = "";
    string _leaveScript = "";
public:
    TrinketCard(string dir, json j, bool isEternal) :
        Card(dir, j),
        _isEternal(isEternal)
    {
        if (j.contains("on")) {
            auto events = j["on"];
            for (const auto& pair : events.items()) {
                auto key = pair.key();
                vector<string> v;
                auto arr = pair.value();
                for (auto const& p : arr.items())
                    v.push_back(fs::readFile(fs::join(dir, p.value()).c_str()));
                _eventMap[key] = v;
            }
        }
        if (j.contains("onEnter"))
            this->_enterScript = fs::readFile(fs::join(dir, j["onEnter"]).c_str());
        if (j.contains("onLeave"))
            this->_leaveScript = fs::readFile(fs::join(dir, j["onLeave"]).c_str());
        if (j.contains("abilities")) {
            for (const auto& jj : j["abilities"].items()) {
                auto a = jj.value();
                ActivatedAbility ability;
                ability.script = fs::readFile(fs::join(dir, a["path"]).c_str());
                for (const auto& c : a["cost"].items())
                    ability.cost.push_back(c.value());
                _abilities.push_back(ability);
            }
        }
    }

    void print(string prefix) {
        std::cout << prefix << _name << std::endl;
        std::cout << prefix << "Is eternal: " << _isEternal << std::endl;
        std::cout << prefix << "Enter script:" << std::endl << prefix << "\t" << _enterScript << std::endl;
        std::cout << prefix << "Leave script:" << std::endl << prefix << "\t" << _leaveScript << std::endl;
        std::cout << prefix << "Event map:" << std::endl;
        for (const auto& [key, value] : _eventMap) {
            std::cout << prefix << "\t" << key << std::endl;
            for (const auto& s : value)
                std::cout << prefix << "\t\t" << s << std::endl;
        }
        std::cout << prefix << "Activations:" << std::endl;
        for (auto& ability : _abilities) {
            ability.print();
        }
    }
};

class LootCard : public Card {
private:
    bool _isTrinket;
    TrinketCard* _trinket = nullptr;

    string _useScript;
public:
    LootCard(string dir, json j, bool isTrinket) : 
        Card(dir, j),
        _isTrinket(isTrinket) 
    {
        if (!_isTrinket) {
            auto usePath = fs::join(dir, j["use"]);
            this->_useScript = fs::readFile(usePath.c_str());
            return;
        }
        this->_trinket = new TrinketCard(dir, j, false);
    }

    ~LootCard() {
        delete _trinket;
    }

    void print() {
        std::cout << _name << std::endl;
        if (_isTrinket) {
            std::cout << "Trinket:" << std::endl;
            this->_trinket->print("\t");
        } else {
            std::cout << "Use: " << std::endl << "\t" << _useScript << std::endl;;
        }
    }
};

class CharacterCard : public Card {
private:
    int _attack;
    int _health;
public:
    CharacterCard(string dir, json j) : 
        Card(dir, j),
        _attack(j["attack"]),
        _health(j["health"]) 
    {
        //  TODO add starting item
    }

    int attack() { return _attack; }
    int health() { return _health; }
};