#include "core.hpp"

#include <iostream>

static const string CARD_INFO_FILE = "card.json";

static const string LOOT_CARDS_FILE = "loot.json";
static const string CHARACTER_CARDS_FILE = "characters.json";
static const string MONSTER_CARDS_FILE = "monsters.json";
static const string TREASURE_CARDS_FILE = "treasures.json";
static const string STARTING_ITEMS_FILE = "starting_items.json";
static const string SETUP_SCRIPT_FILE = "setup.lua";

Game::Game(string dir) {
    this->loadLootCards(dir);
    this->loadTreasureCards(dir);
    this->loadMonsterCards(dir);
    this->loadStartingItems(dir);
    this->loadCharacterCards(dir);

    this->_setupScript = fs::readFile(fs::join(dir, SETUP_SCRIPT_FILE).c_str());
    //  TODO load config
}

string Game::lootCardBackPath(){ return _lootCardBackPath;}
string Game::treasureCardBackPath(){ return _treasureCardBackPath;}
string Game::monsterCardBackPath(){ return _monsterCardBackPath;}

void Game::loadMonsterCards(string dir) {
    auto j = fs::readJS(fs::join(dir, MONSTER_CARDS_FILE));
    this->_monsterCardBackPath = fs::join(dir, j["back"]);
    auto jcards = j["cards"];
    for (const auto& jj : jcards.items()) {
        string tdir = fs::join(dir, jj.value());
        auto jjj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        this->_monsterCards.push_back(new MonsterCard(tdir, jjj));
    }
    // for (const auto& c : _monsterCards)
    //     c->print("");
}

void Game::loadStartingItems(string dir) {
    auto j = fs::readJS(fs::join(dir, STARTING_ITEMS_FILE));
    for (const auto& jj : j.items()) {
        string tdir = fs::join(dir, jj.value());
        auto jjj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        this->_startingItems.push_back(new TrinketCard(tdir, jjj, true));
    }
    // for (const auto& c : _startingItems)
    //     c->print("");
}

void Game::loadLootCards(string dir) {
    auto j = fs::readJS(fs::join(dir, LOOT_CARDS_FILE));
    this->_lootCardBackPath = fs::join(dir, j["back"]);
    auto loadF = [this, j, dir](string key, bool isTrinket) {
        for (const auto& el : j["cards"][key].items()) {
            auto cardPath = fs::join(dir, el.value());
            auto cardJS = fs::readJS(fs::join(cardPath, CARD_INFO_FILE));
            // std::cout << cardJS["name"] << std::endl;
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
    //     c->print("");
}

void Game::loadTreasureCards(string dir) {
    auto j = fs::readJS(fs::join(dir, TREASURE_CARDS_FILE));
    this->_treasureCardBackPath = fs::join(dir, j["back"]);
    auto jcards = j["cards"];
    for (const auto& jj : jcards.items()) {
        string tdir = fs::join(dir, jj.value());
        auto jjj = fs::readJS(fs::join(tdir, CARD_INFO_FILE));
        this->_treasureCards.push_back(new TrinketCard(tdir, jjj, false));
    }
    // for (const auto& c : _treasureCards)
    //     c->print("");
}

void Game::loadCharacterCards(string dir) {
    auto j = fs::readJS(fs::join(dir, CHARACTER_CARDS_FILE));
    for (const auto& el : j.items()) {
        auto character = new CharacterCard(dir, el.value());
        _characterCards.push_back(character);
        auto in = character->startingItemName();
        auto siSet = false;
        for (const auto& item : _startingItems) {
            if (item->name() == in) {
                siSet = true;
                character->setStartingItem(item);
                break;
            }
        }
        if (!siSet) throw std::runtime_error("Unkown starting item: " + in);
    }
}

Game::~Game() {
    for (const auto& c : _characterCards) delete c;
    for (const auto& c : _lootCards) delete c;
    for (const auto& c : _treasureCards) delete c;
    for (const auto& c : _startingItems) delete c;
}

Match* Game::createMatch() {
    auto result = new Match();
    for (auto& card : _characterCards)
        result->addToCharacterPool(card);
    result->createLootDeck(_lootDeckTemplate);
    result->createMonsterDeck(_monsterCards);
    result->createTreasureDeck(_treasureCards);
    result->setupLua(_setupScript);
    return result;
}

std::vector<Card*> Game::getAllCards() {
    vector<Card*> result;
    for (const auto& c : _characterCards) result.push_back(c);
    for (const auto& c : _lootCards) result.push_back(c);
    for (const auto& c : _treasureCards) result.push_back(c);
    for (const auto& c : _startingItems) result.push_back(c);
    for (const auto& c : _monsterCards) result.push_back(c);
    return result;
}
