#pragma once

#include <vector>

#include "cards.hpp"
#include "match.hpp"
#include "../util.hpp"

class Game {
private:
    // character cards
    std::vector<CharacterCard*> _characterCards;
    // loot cards
    std::vector<LootCard*> _lootCards;
    // treasure cards
    std::vector<TrinketCard*> _treasureCards;

    std::vector<std::pair<LootCard*, int>> _lootDeckTemplate;
public:
    Game(string path);
    ~Game();
    void loadLootCards(string dir);
    void loadTreasureCards(string dir);
    void loadCharacterCards(string dir);
    Match* createMatch();
};
