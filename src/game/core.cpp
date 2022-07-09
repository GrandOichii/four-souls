#include "core.hpp"

#include <iostream>

static const string LOOT_CARDS_FILE = "loot.json";
static const string CARD_INFO_FILE = "card.json";

static const string CHARACTER_CARDS_FILE = "characters.json";
static const string TREASURE_CARDS_FILE = "treasures.json";

Game::Game(string dir) {
    this->loadLootCards(dir);
    this->loadTreasureCards(dir);
    //  TODO load monster cards
    this->loadCharacterCards(dir);
    //  TODO load config
    
    // std::cout << "Loot cards:" << std::endl;
    // for (const auto &card : _lootCards)
    //     std::cout << "\t" << card->name() << std::endl;
    // std::cout << "Loot card deck template:" << std::endl;
    // for (const auto &pair : _lootDeckTemplate)
    //     std::cout << "\t" << pair.first << ": " << pair.second << std::endl;
}

void Game::loadLootCards(string dir) {
    auto j = fs::readJS(fs::join(dir, LOOT_CARDS_FILE));

    auto loadF = [this, j, dir](string key, bool isTrinket) {
        for (const auto& el : j["cards"][key].items()) {
            auto cardPath = fs::join(dir, el.value());
            auto cardJS = fs::readJS(fs::join(cardPath, CARD_INFO_FILE));
            std::cout << cardJS["name"] << std::endl;
            auto card = new LootCard(cardPath, cardJS, isTrinket);
            this->_lootCards.push_back(card);
            string name = el.key();
            if (!j["deck"].contains(name)) continue;
            // add cards to the deck template
            int amount = j["deck"][name];
            this->_lootDeckTemplate.push_back(std::make_pair(card, amount));
        }
    };

    loadF("basic", false);
    loadF("trinkets", true);

    // for (const auto& c : _lootCards)
    //     c->print();
}

void Game::loadTreasureCards(string dir) {
    auto j = fs::readJS(fs::join(dir, TREASURE_CARDS_FILE));
    for (const auto& jj : j.items()) {
        string tdir = fs::join(dir, jj.value());
        auto jjj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        this->_treasureCards.push_back(new TrinketCard(tdir, jjj, false));
    }
    // for (const auto& c : _treasureCards)
    //     c->print("");
}

void Game::loadCharacterCards(string dir) {
    auto j = fs::readJS(fs::join(dir, CHARACTER_CARDS_FILE));
    for (const auto& el : j.items())
        _characterCards.push_back(new CharacterCard(dir, el.value()));
}

Game::~Game() {
    for (const auto& c : _characterCards) delete c;
    for (const auto& c : _lootCards) delete c;
    for (const auto& c : _treasureCards) delete c;
}

Match* Game::createMatch() {
    auto result = new Match();
    for (auto& card : _characterCards)
        result->addToCharacterPool(card);
    result->createLootDeck(_lootDeckTemplate);
    return result;
}