#include "match.hpp"

const int MIN_PLAYER_COUNT = 2;
const int MAX_PLAYER_COUNT = 4;
const int SOULS_TO_WIN = 4;
const int STARTING_COIN_AMOUNT = 10;
const int STARTING_LOOT_AMOUNT = 2;
const int STARTING_SHOP_SIZE = 2;
const int STARTING_MONSTERS_AMOUNT = 2;

StackEffect::StackEffect(string funcName, Player* player, CardWrapper* card) :
    funcName(funcName),
    player(player),
    card(card) {}

StackEffect::StackEffect() {}

StackMememberState StackEffect::getState() {
    StackMememberState result;
    result.message = funcName;
    result.isCard = card;
    if (card) result.cardName = card->card()->name();
    return result;
}

Match::Match() {
    this->rng.seed(rand());
}

Match::~Match() {
    for (const auto& p : _players) delete p;

    lua_close(this->L);
}

void Match::addToDiscard(LootCard* card) {
    this->_lootDiscard.push_back(card);
}

CardWrapper* Match::cardWithID(int id) {
    for (const auto& p : _players)
        for (const auto& w : p->board())
            if (w->id() == id)
                return w;
    throw std::runtime_error("no card with id " + std::to_string(id));
}

Player* Match::findOwner(CardWrapper* card) {
    for (const auto& p : _players)
        for (const auto& w : p->board())
            if (w == card)
                return p;
    throw std::runtime_error("can't find owner of card " + card->card()->name() + " [" + std::to_string(card->id()) + "]");
} 

void Match::shuffleLootDiscardIntoMain() {
    while (!_lootDiscard.empty()) {
        _lootDeck.push_back(_lootDiscard.back());
        _lootDiscard.pop_back();
    }
    this->shuffleLootDeck();
}

void Match::shuffleTreasureDiscardIntoMain() {
    while (!_treasureDiscard.empty()) {
        _treasureDeck.push_back(_treasureDiscard.back());
        _treasureDiscard.pop_back();
    }
    this->shuffleTreasureDeck();
}

void Match::shuffleMonsterDiscardIntoMain() {
    while (!_monsterDiscard.empty()) {
        _monsterDeck.push_back(_monsterDiscard.back());
        _monsterDiscard.pop_back();
    }
    this->shuffleMonsterDeck();
}

LootCard* Match::getTopLootCard() {
    if (!_lootDeck.size()) this->shuffleLootDiscardIntoMain();
    if (_lootDeck.empty()) return nullptr;
    return _lootDeck.back();
}

vector<LootCard*> Match::getTopLootCards(int amount) {
    vector<LootCard*> cards;
    while (amount) {
        auto card = getTopLootCard();
        if (!card) {
            this->log("The loot deck is empty");
            break;
        }
        cards.push_back(card);
        // cards.push_back(_lootDeck.back());
        _lootDeck.pop_back();
        amount--;
    }
    return cards;
}

TrinketCard* Match::getTopTreasureCard() {
    if (!_treasureDeck.size()) this->shuffleTreasureDiscardIntoMain();
    if (_treasureDeck.empty()) return nullptr;
    return _treasureDeck.back();
}

MonsterCard* Match::getTopMonsterCard() {
    if (!_monsterDeck.size()) this->shuffleMonsterDiscardIntoMain();
    if (_monsterDeck.empty()) return nullptr;
    return _monsterDeck.back();
}

vector<TrinketCard*> Match::getTopTreasureCards(int amount) {
    vector<TrinketCard*> cards;
    while (amount) {
        auto card = getTopTreasureCard();
        if (!card) {
            this->log("The treasure deck is empty");
            break;
        }
        cards.push_back(card);
        _treasureDeck.pop_back();
        amount--;
    }
    return cards;
}

vector<MonsterCard*> Match::getTopMonsterCards(int amount) {
    vector<MonsterCard*> cards;
    while (amount) {
        auto card = getTopMonsterCard();
        if (!card) {
            this->log("The monster deck is empty");
            break;
        }
        cards.push_back(card);
        _monsterDeck.pop_back();
        amount--;
    }
    return cards;
}

int Match::wrap_getOwner(lua_State *L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    match->_stack.back().player->pushTable(L);
    return 1;
}

// int Match::wrap_getPlayers(lua_State *L) {
    
// }

int Match::wrap_addCoins(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    auto amount = (int)lua_tonumber(L, 3);
    Player* player = nullptr;
    for (const auto& p : match->_players) {
        if (p->id() == pid) {
            player = p;
            break;
        }
    }
    if (!player) throw std::runtime_error("no player with id " + std::to_string(pid));
    match->log(player->name() + " gains " + std::to_string(amount) + " coins");
    player->addCoins(amount);
    return 0;
}

int Match::wrap_buyItem(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    TrinketCard* card = nullptr;
    auto top = match->getTopTreasureCard();
    match->_treasureDeck.pop_back();
    if (match->_lastTreasureIndex == -1) {
        card = top;
    } else {
        card = match->_shop[match->_lastTreasureIndex];
        match->_shop[match->_lastTreasureIndex] = top;
    }
    auto player = match->_stack.back().player;
    player->payPricePerTreasure();
    auto w = new CardWrapper(card, match->newCardID());
    player->addToBoard(w);
    auto efn = card->enterFuncName();
    if (!efn.size()) return 0;
    std::cout << efn << std::endl;
    match->execFunc(efn);
    return 0;
}

int Match::wrap_lootCards(lua_State *L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    auto amount = (int)lua_tonumber(L, 3);
    Player* player = match->playerWithID(pid);
    match->log(player->name() + " loots " + std::to_string(amount) + " cards");
    auto cards = match->getTopLootCards(amount);
    player->addLootCards(cards);
    return 0;
}

int Match::wrap_popLootStack(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    auto last = match->_lootStack.top();
    match->_lootStack.pop();
    auto card = last.first;
    auto player = last.second;
    card->use(player, match);
    return 0;
}

int Match::wrap_deferEOT(lua_State *L) {
    if (lua_gettop(L) != 4) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto cardID = (int)lua_tonumber(L, 2);
    auto card = match->cardWithID(cardID);
    auto owner = match->findOwner(card);
    if (!lua_isstring(L, 3)) {
        lua_err(L);
        exit(1);
    }
    auto funcName = (string)lua_tostring(L, 3);
    if (!lua_isboolean(L, 4)) {
        lua_err(L);
        exit(1);
    }
    auto isTrigger = (bool)lua_toboolean(L, 4);
    if (isTrigger) {
        StackEffect effect;
        effect.funcName = funcName;
        effect.card = card;
        effect.player = owner;
        match->_eotDeferredTriggers.push(effect);
    } else match->_eotDefers.push(funcName);
    return 0;
}

int Match::wrap_this(lua_State *L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    match->_stack.back().card->pushTable(match->L);
    return 1;
}

int Match::wrap_incBeginningLoot(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    std::cout << "Executed\n";
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int pid = (int)lua_tonumber(L, 2);
    auto player = match->playerWithID(pid);
    player->incBeginningLoot();
    return 0;
}

int Match::wrap_decBeginningLoot(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int pid = (int)lua_tonumber(L, 2);
    auto player = match->playerWithID(pid);
    player->decBeginningLoot();
    return 0;
}

Player* Match::playerWithID(int id) {
    for (const auto& p : this->_players) 
        if (p->id() == id) 
            return p;
    throw std::runtime_error("no player with id " + std::to_string(id));
}

void Match::pushEOTDeferredTriggers() {
    while (!_eotDeferredTriggers.empty()) {
        this->pushToStack(_eotDeferredTriggers.top());
        _eotDeferredTriggers.pop();
    }
}

void Match::execEOTDefers() {
    while (!_eotDefers.empty()) {
        this->execFunc(_eotDefers.top());
        _eotDefers.pop();
    }
}

void Match::setupLua() {
    this->L = luaL_newstate();
    // connect common libs
    luaL_openlibs(L);
    // connect functions
    lua_register(L, "getOwner", wrap_getOwner);
    lua_register(L, "lootCards", wrap_lootCards);
    lua_register(L, "addCoins", wrap_addCoins);
    lua_register(L, "buyItem", wrap_buyItem);
    lua_register(L, "deferEOT", wrap_deferEOT);
    lua_register(L, "_popLootStack", wrap_popLootStack);
    lua_register(L, "incBeginningLoot", wrap_incBeginningLoot);
    lua_register(L, "decBeginningLoot", wrap_decBeginningLoot);
    lua_register(L, "this", wrap_this);

    // load card scripts
    for (const auto& card : _lootDeck) {
        std::cout << "Loading script for " << card->name() << std::endl;
        this->execScript(card->script());
    }
    for (const auto& card : _treasureDeck) {
        std::cout << "Loading script for " << card->name() << std::endl;
        this->execScript(card->script());
    }
    for (const auto& pair : _characterPool) {
        auto character = pair.first;
        auto card = character->startingItem();
        std::cout << "Loading script for " << card->name() << std::endl;
        this->execScript(card->script());
    }
    std::cout << "All scripts loaded!" << std::endl;
}

void Match::lua_err(lua_State *L) {
    string errormsg = lua_tostring(L, -1);
    std::cout << errormsg << std::endl;
}

void Match::execScript(string script) {
    int r = luaL_dostring(L, script.c_str());
    if (r != LUA_OK) {
        lua_err(this->L);
        exit(1);
    }
}

void Match::execFunc(string funcName) {
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1)) {
        lua_err(L);
        exit(1);
    }
    lua_pushlightuserdata(L, this);
    int r = lua_pcall(L, 1, 0, 0);
    if (r != LUA_OK) {
        lua_err(this->L);
        exit(1);
    }
}

bool Match::execCheck(string funcName) {
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1)) {
        lua_err(L);
        exit(1);
    }
    lua_pushlightuserdata(L, this);
    int r = lua_pcall(L, 1, 1, 0);
    if (r != LUA_OK) {
        lua_err(this->L);
        exit(1);
    }
    if (!lua_isboolean(L, -1)) {
        lua_err(this->L);
        exit(1);
    }
    return (bool)lua_toboolean(L, -1);
}

void Match::addToCharacterPool(CharacterCard* card) {
    _characterPool.push_back(std::make_pair(card, true));
}

//  TODO remove the actions part
Player* Match::addPlayer(std::string name, CharacterCard* character, string actions) {
    for (const auto& p : _players)
        if (p->name() == name)
            return nullptr;
    //  TODO change this to a player with a port
    auto result = new ScriptedPlayer(name, character, newCardID(), actions);
    result->addCoins(STARTING_COIN_AMOUNT);
    _players.push_back(result);
    return result;
}

std::vector<CharacterCard*> Match::getAvailableCharacters() {
    std::vector<CharacterCard*> result;
    for (const auto& pair : _characterPool)
        if (pair.second)
            result.push_back(pair.first);
    return result;
}

CharacterCard* Match::getRandomAvailableCharacter() {
    auto characters = this->getAvailableCharacters();
    int top = characters.size();
    int i = rand() % top;
    auto result = characters[i];
    for (auto& pair : _characterPool) {
        if (pair.first == result) {
            pair.second = false;
            break;
        }
    }
    return result;
}

void Match::shuffleLootDeck() {
    std::shuffle(std::begin(_lootDeck), std::end(_lootDeck), rng);
}

void Match::shuffleTreasureDeck() {
    std::shuffle(std::begin(_treasureDeck), std::end(_treasureDeck), rng);
}

void Match::shuffleMonsterDeck() {
    std::shuffle(std::begin(_monsterDeck), std::end(_monsterDeck), rng);
}

void Match::createLootDeck(std::vector<std::pair<LootCard*, int>> pairs) {
    for (const auto& pair : pairs)
        for (int i = 0; i < pair.second; i++)
            this->_lootDeck.push_back(pair.first);
    this->shuffleLootDeck();
}

void Match::createTreasureDeck(std::vector<TrinketCard*> cards) {
    for (const auto& card : cards)
        this->_treasureDeck.push_back(card);            
    this->shuffleTreasureDeck();
}

void Match::createMonsterDeck(std::vector<MonsterCard*> cards) {
    for (const auto& card : cards)
        this->_monsterDeck.push_back(card);            
    this->shuffleMonsterDeck();
}

void Match::start() {
    std::cout << "\nThe game starts\n\n";
    // setup script
    this->execScript("function _startTurnLoot(host)\n\tlocal owner = getOwner(host)\nlootCards(host, owner[\"id\"], owner[\"startTurnLootAmount\"])\nend");
    // give starting hands
    for (auto& p : _players) {
        auto cards = this->getTopLootCards(STARTING_LOOT_AMOUNT);
        p->addLootCards(cards);
    }
    // setup shop
    auto tcards = getTopTreasureCards(STARTING_SHOP_SIZE);
    for (const auto& c : tcards)
        _shop.push_back(c);
    // setup monsters
    auto mcards = getTopMonsterCards(STARTING_MONSTERS_AMOUNT);
    for (const auto& c : mcards)
        _monsters.push_back(c);

    // std::cout << "Gave the starting hands" << std::endl;
    // std::cout << "Loot deck:" << std::endl;
    // for (const auto& card : _lootDeck)
    //     std::cout << "\t" << card->name() << std::endl;
    // std::cout << "Treasure deck:" << std::endl;
    // for (const auto& card : _treasureDeck)
    //     std::cout << "\t" << card->name() << std::endl;
    // std::cout << "Players:" << std::endl;
    // for (const auto& p : _players)
    //     p->print();

    // this->_currentI = rand() % _players.size();
    this->_currentI = 0;

    this->_priorityI = this->_currentI;
    this->_running = true;

    int c = 10;
    while (this->_running) {
        this->calcNext();
        this->turn();
        this->passTurn();
        c--;
        if (!c) break;
    }   
}

void Match::passTurn() {
    this->_currentI = this->_nextI;
}

void Match::calcNext() {
    this->_nextI = (this->_currentI + 1) % _players.size();
}

void Match::currentPlayerLoot() {
    this->pushToStack(StackEffect(
        "_startTurnLoot", 
        _activePlayer, 
        nullptr
    ));
}

void Match::turn() {
    this->log("TURN " + std::to_string(++this->_turnCounter) + " - ");
    this->_activePlayer = this->_players[this->_currentI];
    this->log(this->_activePlayer->name() + " starts their turn");

    // recharge all of the items and character card of the active player
    this->_activePlayer->recharge();

    // apply all <start of turn> triggers, then resolve stack
    this->applyTriggers(TURN_START_TRIGGER);
    this->resolveStack();

    // add loot 1 to stack, then resolve stack
    this->currentPlayerLoot();
    this->resolveStack();

    // main step
    this->log(_activePlayer->name() + "'s main phase");
    string response = "";
    while ((response = this->_activePlayer->promptAction()) != ACTION_PASS) {
        std::cout << "\t" << _activePlayer->name() << ": " << response << std::endl;
        this->executePlayerAction(_activePlayer, response);
        this->resolveStack();
    }
    this->log("End of " + this->_activePlayer->name() + "'s turn");

    // end of turn
    //  TODO prompt the player to choose the order of triggers
    this->applyTriggers(TURN_END_TRIGGER);
    this->pushEOTDeferredTriggers();
    this->resolveStack();

    // all silent end of turn effects
    this->execEOTDefers();
}

void Match::executePlayerAction(Player* player, string action) {
    auto split = str::split(action, " ");
    if (!this->_actionMap.count(split[0])) throw std::runtime_error("don't have a handler for " + split[0] + " action in match");
    this->_actionMap[split[0]](player, split);
    // this->pushToStack(StackEffect(
    //     "_customAction",
    //     player,
    //     nullptr
    // ));
}

void Match::applyTriggers(string triggerType) {
    // first the monsters

    // then the items, starting with the current player
    //  TODO prompt the player to order them
    for (int i = 0; i < _players.size(); i++) {
        auto player = _players[(_currentI + i) % _players.size()];
        auto board = player->board();
        for (const auto& w : board) {
            auto card = (TrinketCard*)w->card();
            if (!card->hasTrigger(triggerType)) continue;
            // std::cout << "Card " << card->name() << "[" << w->id() << "] has a " << triggerType << " trigger" << std::endl;
            auto pair = card->getTriggerWhen(triggerType);
            auto checkFuncName = pair.first;
            if (!this->execCheck(checkFuncName)) continue;
            this->log(card->name() + " is triggered.");
            this->pushToStack(StackEffect(pair.second, player, w));
        }
    }
}

void Match::pushToStack(const StackEffect& effect) {
    this->_stack.push_back(effect);
}

void Match::resolveStack() {
    // while the stack isn't empty
    while (!this->_stack.empty()) {
        this->resolveTop();
    }
}

void Match::resolveTop() {
    auto effect = this->_stack.back();
    // pass the priority of the owner of the effect
    // this->_priorityI = -1;
    // for (int i = 0; i < _players.size(); i++) {
    //     if (_players[i] == effect.player) {
    //         this->_priorityI = i;
    //         break;
    //     }
    // }
    this->_priorityI = this->_currentI;
    const auto last = this->_currentI;
    // if (this->_priorityI == -1) throw std::runtime_error("Unknown player " + effect.player->name());
    do {
        // prompt action
        auto response = this->promptPlayerWithPriority();
        if (response == ACTION_PASS) {
            // player passes priority
            this->log(this->_players[this->_priorityI]->name() + " passes priority");
            this->_priorityI = (this->_priorityI + 1) % _players.size();
            continue;
        }
        // player put something on stack
        return;
    } while (last != this->_priorityI);
    // resolve the ability
    this->execFunc(effect.funcName);
    this->_stack.pop_back();
}

string Match::promptPlayerWithPriority() {
    auto player = this->_players[this->_priorityI];
    this->log("Player " + player->name() + " takes priority");
    return player->promptAction();
}

void Match::log(string message) {
    std::cout << " - " << message << std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

MatchState Match::getState() {
    MatchState result;
    for (const auto& p : _players) 
        result.boards.push_back(p->getState());
    for (auto& si : _stack)
        result.stack.push_back(si.getState());
    result.currentI = _currentI;
    result.priorityI = _priorityI;

    result.lootDeckCount = _lootDeck.size();
    for (const auto& c : _lootDiscard)
        result.lootDiscard.push_back(c->name());
    
    result.treasureDeckCount = _treasureDeck.size();
    for (const auto& c : _treasureDiscard)
        result.treasureDiscard.push_back(c->name());
    for (const auto& c : _shop)
        result.shop.push_back(c->name());

    result.monsterDeckCount = _monsterDeck.size();
    for (const auto& c : _monsterDiscard)
        result.monsterDiscard.push_back(c->name());
    for (const auto& c : _monsters)
        result.monsters.push_back(c->name());

    return result;
}

int Match::newCardID() {
    return ++_lastID;
}