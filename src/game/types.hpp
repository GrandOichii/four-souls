#pragma once

#include <string>
#include <iostream>

#include "../util.hpp"

using std::string;

const string DEATH_TRIGGER = "death";
const string DAMAGE_TRIGGER = "damage";
const string TURN_START_TRIGGER = "turn_start";
const string TURN_END_TRIGGER = "turn_end";

const string CARD_TYPE_LOOT = "loot";
const string CARD_TYPE_TREASURE = "treasure";
const string CARD_TYPE_MONSTER = "monster";

const string ACTION_PLAY_LOOT = "play_loot"; // [0] - the index of the card 
const string ACTION_ATTACK_MONSTER = "attack"; // [0] - the index of the card being attacked, if -1 then the top
const string ACTION_BUY_TREASURE = "buy_treasure"; // [0] - the index of the card being purchased, if -1 then the top
const string ACTION_ACTIVATE_CARD = "activate";
const string ACTION_ACTIVATE_CHARACTER_CARD = "activate_character";

const string LOOT_TYPE = "loot";
const string PLAY_LOOT_CARD_TYPE = "loot_card";
const string REWARDS_TYPE = "rewards";
const string STACK_MEMBER_TYPE = "stack_member";
const string ROLL_TYPE = "roll";
const string COMBAT_DAMAGE_TYPE = "combat_damage";
const string DEATH_TYPE = "death";
const string TRIGGER_TYPE = "trigger";
const string BUY_TREASURE_TYPE = "buy_treasure";
const string ATTACK_MONSTER_TYPE = "attack";
const string ACTIVATE_ITEM_TYPE = "activate";
const string ACTIVATE_CHARACTER_TYPE = "activate_character";

const string LOOT_DECK = "loot";
const string TREASURE_DECK = "treasure";
const string MONSTER_DECK = "monster";

const string PLAYER_TARGET = "player";
const string CARD_TARGET = "card";
const string STACK_MEMBER_TARGET = "stack_member";
const string MONSTER_TARGET = "monster";

const string PLAYER_TYPE = "player";
const string MONSTER_TYPE = "monster";
const string CARD_TYPE = "card";

const string RESPONSE_CANCEL = "$CANCEL";
const string RESPONSE_FIRST = "$FIRST";

const string ACTION_PASS = "$PASS";

struct ActivatedAbility {
    string costFuncName;
    string funcName;

    void print() {
        std::cout << "Cost: " << costFuncName << std::endl;
        std::cout << "funcName: " << funcName << std::endl;
    }
};

enum Zones : int {
    Unknown,
    Hand,
    Board,
    CCard,
    Shop,
    Stack,
    LootDiscard,
    TreasureDiscard,
    MonsterDiscard,
    ActiveMonsters
};