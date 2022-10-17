#include "core.hpp"

#include <iostream>

static const string CONFIG_FILE = "config.json";
static const string LOOT_CARDS_FILE = "loot.json";
static const string CHARACTER_CARDS_FILE = "characters.json";
static const string MONSTER_CARDS_FILE = "monsters.json";
static const string TREASURE_CARDS_FILE = "treasures.json";
static const string BONUS_SOULS_FILE = "bonus_souls.json";
static const string STARTING_ITEMS_FILE = "starting_items.json";
static const string SETUP_SCRIPT_FILE = "setup.lua";

Game::Game(string dir) {
    this->loadLootCards(dir);
    this->loadTreasureCards(dir);
    this->loadMonsterCards(dir);
    this->loadCharacterCards(dir);
    this->loadBonusSouls(dir);

    this->_setupScript = fs::readFile(fs::join(dir, SETUP_SCRIPT_FILE).c_str());
    this->_matchConfig = fs::readJS(fs::join(dir, CONFIG_FILE).c_str());
}

void Game::loadMonsterCards(string dir) {
    auto j = fs::readJS(fs::join(dir, MONSTER_CARDS_FILE));
    auto jcards = j["cards"];
    auto mcards = jcards["monsters"];
    for (const auto& jj : mcards.items()) {
        string tdir = fs::join(dir, jj.value());
        auto jjj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        try {
            this->_monsterCards.push_back(new MonsterCard(tdir, jjj));
        } catch (std::exception& ex) {
            std::cerr << "Failed loading card from " << tdir << std::endl;
            throw ex;
        }

    }
    auto bcards = jcards["bonus"];
    for (const auto& jj : bcards.items()) {
        string tdir = fs::join(dir, jj.value());
        auto jjj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        try {
            this->_bonusMonsterCards.push_back(new ScriptCard(tdir, jjj, CardTypes::BonusMonster));
        } catch (std::exception& ex) {
            std::cerr << "Failed loading card from " << tdir << std::endl;
            throw ex;
        }
    }
    // for (const auto& c : _monsterCards)
    //     c->print("");
}

void Game::loadBonusSouls(string dir) {
    auto j = fs::readJS(fs::join(dir, BONUS_SOULS_FILE));
    for (const auto& el : j.items()) {
        string tdir = fs::join(dir, el.value()); 
        auto jj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        try {
            auto card = new ScriptCard(tdir, jj, CardTypes::BonusSoul);
            _bonusSouls.push_back(card);
        } catch (std::exception& ex) {
            std::cerr << "Failed loading card from " << tdir << std::endl;
            throw ex;
        }

    }
}

void Game::loadLootCards(string dir) {
    auto j = fs::readJS(fs::join(dir, LOOT_CARDS_FILE));
    auto jcards = j["cards"];
    for (const auto& jj : jcards.items()) {
        string tdir = fs::join(dir, jj.key());
        auto jjj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        try {
            auto card = new ScriptCard(tdir, jjj, CardTypes::Loot);
            this->_lootCards.push_back(card);
            this->_lootDeckTemplate.push_back(std::make_pair(card, jj.value()));
        } catch (std::exception& ex) {
            std::cerr << "Failed loading card from " << tdir << std::endl;
            throw ex;
        }

    }
}

void Game::loadTreasureCards(string dir) {
    auto j = fs::readJS(fs::join(dir, TREASURE_CARDS_FILE));
    auto jcards = j["cards"];
    for (const auto& jj : jcards.items()) {
        string tdir = fs::join(dir, jj.value());
        auto jjj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        try {
            this->_treasureCards.push_back(new ScriptCard(tdir, jjj, CardTypes::Treasure));
        } catch (std::exception& ex) {
            std::cerr << "Failed loading card from " << tdir << std::endl;
            throw ex;
        }
    }
    // for (const auto& c : _treasureCards)
    //     c->print("");
}

void Game::loadCharacterCards(string dir) {
    auto j = fs::readJS(fs::join(dir, CHARACTER_CARDS_FILE));
    for (const auto& el : j.items()) {
        string tdir = fs::join(dir, el.value()); 
        auto jj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        try {
            auto character = new CharacterCard(tdir, jj);
            _characterCards.push_back(character);
        } catch (std::exception& ex) {
            std::cerr << "Failed loading card from " << tdir << std::endl;
            throw ex;
        }

    }
}

Game::~Game() {
    for (const auto& c : _characterCards) delete c;
    for (const auto& c : _lootCards) delete c;
    for (const auto& c : _treasureCards) delete c;
    for (const auto& c : _bonusSouls) delete c;
}

Match* Game::createMatch(){ return createMatch(time(0)); }

Match* Game::createMatch(int seed) {
    auto result = new Match(_matchConfig, seed);
    for (auto& card : _characterCards)
        result->addToCharacterPool(card);
    result->createLootDeck(_lootDeckTemplate);
    result->createMonsterDeck(_monsterCards, _bonusMonsterCards);
    result->createTreasureDeck(_treasureCards);
    result->createBonusSouls(_bonusSouls);
    result->setupLua(_setupScript);
    return result;
}

std::vector<Card*> Game::getAllCards() {
    vector<Card*> result;
    for (const auto& c : _characterCards) {
        result.push_back(c);
        auto si = c->startingItem();
        if (si) result.push_back(si);
    }
    for (const auto& c : _lootCards) result.push_back(c);
    for (const auto& c : _treasureCards) result.push_back(c);
    for (const auto& c : _monsterCards) result.push_back(c);
    for (const auto& c : _bonusSouls) result.push_back(c);
    return result;
}
