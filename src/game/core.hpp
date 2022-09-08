#pragma once

#include <vector>

#include "cards.hpp"
#include "match.hpp"
#include "../util.hpp"

class Game {
private:
    nlohmann::json _matchConfig;

    // character cards
    std::vector<CharacterCard*> _characterCards;
    // loot cards
    std::vector<ScriptCard*> _lootCards;
    // treasure cards
    std::vector<ScriptCard*> _treasureCards;
    // monster cards
    std::vector<MonsterCard*> _monsterCards;
    // bonus monster cards
    std::vector<ScriptCard*> _bonusMonsterCards;
    // bonus souls
    std::vector<ScriptCard*> _bonusSouls;

    std::vector<std::pair<ScriptCard*, int>> _lootDeckTemplate;

    string _setupScript;
public:
    Game(string path);
    ~Game();
    void loadLootCards(string dir);
    void loadTreasureCards(string dir);
    void loadMonsterCards(string dir);
    void loadCharacterCards(string dir);
    void loadBonusSouls(string dir);
    Match* createMatch();
    Match* createMatch(int seed);
    std::vector<Card*> getAllCards();
};
