#pragma once

#include <string>
#include <iostream>

#include "../util.hpp"

using std::string;

const string ROLL_TRIGGER = "roll";
const string DEATH_TRIGGER = "death";
const string TURN_START_TRIGGER = "turn_start";
const string TURN_END_TRIGGER = "turn_end";

const string CARD_TYPE_LOOT = "loot";
const string CARD_TYPE_TREASURE = "treasure";
const string CARD_TYPE_MONSTER = "monster";

const string ACTION_PLAY_LOOT = "play_loot"; // [0] - the index of the card 
const string ACTION_ATTACK_MONSTER = "attack_monster"; // [0] - the index of the card being attacked, if -1 then the top
const string ACTION_BUY_TREASURE = "buy_treasure"; // // [0] - the index of the card being purchased, if -1 then the top

const string ACTION_PASS = "$PASS";

struct ActivatedAbility {
    std::vector<string> cost;
    string funcName;

    void print() {
        std::cout << "Cost: " << str::join(cost.begin(), cost.end(), ", ") << std::endl;
        std::cout << "funcName: " << funcName << std::endl;
    }
};
