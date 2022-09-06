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

const string ACTION_PLAY_LOOT = "play_loot"; 
const string ACTION_ATTACK_MONSTER = "attack";
const string ACTION_BUY_TREASURE = "buy_treasure";
const string ACTION_ACTIVATE_CARD = "activate";
const string ACTION_ACTIVATE_CHARACTER_CARD = "activate_character";

const string LOOT_TYPE = "loot";
const string PLAY_LOOT_CARD_TYPE = "loot_card";
const string REWARDS_TYPE = "rewards";
const string STACK_MEMBER_TYPE = "stack_member";
const string ROLL_TYPE = "roll";
const string POST_ROLL_TYPE = "post_roll";
const string COMBAT_DAMAGE_TYPE = "combat_damage";
const string DEATH_TYPE = "death";
const string COIN_GAIN_TYPE = "coin_gain";
const string LOOT_GAIN_TYPE = "loot_gain";
const string AFTER_DEATH_TYPE = "after_death";
const string TRIGGER_TYPE = "trigger";
const string BUY_TREASURE_TYPE = "buy_treasure";
const string ATTACK_MONSTER_TYPE = "attack";
const string ACTIVATE_ITEM_TYPE = "activate";
const string ACTIVATE_CHARACTER_TYPE = "activate_character";
const string COMBAT_END_TYPE = "combat_end";
const string MONSTER_DEATH_TRIGGER_TYPE = "monster_death_trigger";
const string MONSTER_ENTER_TYPE = "monster_enter";
const string CHARACTER_ENTER_TYPE = "character_enter";
const string BONUS_SOUL_ENTER_TYPE = "bonus_soul_enter";
const string MONSTER_LEAVE_TYPE = "monster_leave";
const string ITEM_ENTER_TYPE = "item_enter";
const string ITEM_LEAVE_TYPE = "item_leave";
const string ITEM_DESTROYED_TYPE = "item_destroyed";
const string REFILL_MONSTERS_TYPE = "refill_monsters";
const string REFILL_SHOP_TYPE = "refill_shop";

const string LOOT_DECK = "loot";
const string TREASURE_DECK = "treasure";
const string MONSTER_DECK = "monster";

const string ROLL_REQUIREMENT = "roll";
const string TAP_REQUIREMENT = "tap";

const string PLAYER_TARGET = "player";
const string CARD_TARGET = "card";
const string SHOP_CARD_TARGET = "shop_card";
const string STACK_MEMBER_TARGET = "stack_member";
const string MONSTER_TARGET = "monster";
const string SOUL_TARGET = "soul";

const string PLAYER_TYPE = "player";
const string MONSTER_TYPE = "monster";
const string CARD_TYPE = "card";

const string RESPONSE_CANCEL = "$CANCEL";
const string RESPONSE_FIRST = "$FIRST";

const string ACTION_PASS = "$PASS";

enum Zones : int {
    Unknown,
    Hand,
    Souls,
    Board,
    CCard,
    Shop,
    Stack,
    LootDiscard,
    TreasureDiscard,
    MonsterDiscard,
    ActiveMonsters
};

enum CardTypes : int {
    Loot,
    Treasure,
    Monster,
    Character,
    BonusSoul,
    StartingItem
};

const std::map<std::string, CardTypes> CARD_TYPE_MAP = {
    {"loot", CardTypes::Loot},
    {"treasure", CardTypes::Treasure},
    {"monster", CardTypes::Monster},
    {"character", CardTypes::Character}
};

class Match;
struct StackEffect;
class CardWrapper;
class Player;

struct EffectState {
    string text = "";
};

struct Effect {
    string costFuncName = "";
    string funcName = "";
    string text = "";
    bool usesStack = true;
    vector<string> requirements;

    Effect();
    Effect(json j);

    bool hasRequirement(string req);
    EffectState getState() const;
    virtual StackEffect* pushMe(Match* match, CardWrapper* cardW, Player* owner, string type);
};

struct Trigger : public Effect {
    string checkFuncName;

    Trigger();
    Trigger(json j);
    StackEffect* pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) override;
};