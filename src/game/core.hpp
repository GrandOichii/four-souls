#pragma once

#include <vector>

#include "cards.hpp"
#include "match.hpp"
#include "../util.hpp"

class Game {
private:
    // starting items
    std::vector<TrinketCard*> _startingItems;
    // character cards
    std::vector<CharacterCard*> _characterCards;
    // loot cards
    std::vector<LootCard*> _lootCards;
    // treasure cards
    std::vector<TrinketCard*> _treasureCards;

    std::vector<std::pair<LootCard*, int>> _lootDeckTemplate;

    //  TODO move this to client
    // card backs
    string _lootCardBackPath;
    string _treasureCardBackPath;
public:
    Game(string path);
    ~Game();
    string lootCardBackPath();
    string treasureCardBackPath();
    void loadLootCards(string dir);
    void loadTreasureCards(string dir);
    void loadCharacterCards(string dir);
    void loadStartingItems(string dir);
    Match* createMatch();
    std::vector<Card*> getAllCards();
};
