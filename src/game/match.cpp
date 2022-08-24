#include "match.hpp"

// #include "../nlohmann/json.hpp"

MatchState::MatchState() {}

using namespace nlohmann;

static CardState cardFromJson(json j) {
    CardState result;
    result.cardName = j["cardName"];
    result.active = j["active"];
    result.id = j["id"];
    result.counters = j["counters"];
    result.zone = j["zone"];
    result.ownerID = j["ownerID"];
    result.soulCount = j["soulCount"];
    result.activatedAbilityCount = j["activatedAbilityCount"];
    return result;
}

static vector<CardState> cardVectorFromJson(json j) {
    vector<CardState> result;
    for (const auto& [key, value] : j.items())
        result.push_back(cardFromJson(value));
    return result;
}

static PlayerBoardState playerFromJson(json j) {
    PlayerBoardState result;
    result.coinCount = j["coinCount"];
    result.characterActive = j["characterActive"];
    result.health = j["health"];
    result.maxHealth = j["maxHealth"];
    result.blueHealth = j["blueHealth"];
    result.treasurePrice = j["treasurePrice"];
    result.attack = j["attack"];
    result.health = j["health"];
    result.playableCount = j["playableCount"];
    result.purchaseCount = j["purchaseCount"];
    result.attackCount = j["attackCount"];
    result.isDead = j["isDead"];
    result.id = j["id"];
    result.name = j["name"];

    result.playerCard = cardFromJson(j["playerCard"]);
    result.board = cardVectorFromJson(j["board"]);
    result.hand = cardVectorFromJson(j["hand"]);
    result.souls = cardVectorFromJson(j["souls"]);
    return result;
}

static MonsterDataState monsterDataFromJson(json j) {
    MonsterDataState result;
    result.roll = j["roll"];
    result.power = j["power"];
    result.health = j["health"];
    result.blueHealth = j["blueHealth"];
    result.canBeAttacked = j["canBeAttacked"];
    return result;
}

static StackMemberState stackMemberFromJson(json j) {
    StackMemberState result;
    result.message = j["message"];
    result.isCard = j["isCard"];
    result.type = j["type"];
    result.card = cardFromJson(j["card"]);
    for (const auto& [_, target] : j["targets"].items()) {
        result.targets.push_back(target);
    }
    return result;
}

static RollEventState rollStackMemberFromJson(json j) {
    RollEventState result;
    result.value = j["value"];
    result.isCombatRoll = j["isCombatRoll"];
    result.ownerID = j["ownerID"];
    return result;
}

MatchState::MatchState(nlohmann::json j){
    for (const auto& [key, value] : j["boards"].items())
        boards.push_back(playerFromJson(value));
    for (const auto& [key, value] : j["stack"].items())
        stack.push_back(stackMemberFromJson(value));
    for (const auto& [key, value] : j["rollStack"].items())
        rollStack.push_back(rollStackMemberFromJson(value));
    for (const auto& [key, value] : j["monsterData"].items())
        monsterDataArr.push_back(monsterDataFromJson(value));
    turnCounter = j["turnCounter"];
    currentI = j["currentI"];
    priorityI = j["priorityI"];
    currentID = j["currentID"];
    isMain = j["isMain"];
    lootDeckCount = j["lootDeckCount"];
    treasureDeckCount = j["treasureDeckCount"];
    monsterDeckCount = j["monsterDeckCount"];
    lootDiscard = cardVectorFromJson(j["lootDiscard"]);
    treasureDiscard = cardVectorFromJson(j["treasureDiscard"]);
    monsterDiscard = cardVectorFromJson(j["monsterDiscard"]);
    shop = cardVectorFromJson(j["shop"]);
    monsters = cardVectorFromJson(j["monsters"]);
    isCombat = j["isCombat"];
}

static json cardToJson(const CardState& card) {
    json result;
    result["cardName"] = card.cardName;
    result["active"] = card.active;
    result["id"] = card.id;
    result["counters"] = card.counters;
    result["zone"] = card.zone;
    result["ownerID"] = card.ownerID;
    result["soulCount"] = card.soulCount;
    result["activatedAbilityCount"] = card.activatedAbilityCount;
    return result;
}

static json monsterDataToJson(const MonsterDataState& data) {
    json result;
    result["health"] = data.health;
    result["power"] = data.power;
    result["roll"] = data.roll;
    result["blueHealth"] = data.blueHealth;
    result["canBeAttacked"] = data.canBeAttacked;
    return result;
}

static json cardVectorToJson(const vector<CardState>& vec) {
    json result = json::array();
    for (const auto& card : vec) {
        result.push_back(cardToJson(card));
    }
    return result;
}

static json stackMemberToJson(const StackMemberState& member) {
    json result;
    result["message"] = member.message;
    result["isCard"] = member.isCard;
    result["type"] = member.type;
    result["card"] = cardToJson(member.card);
    result["targets"] = json::array();
    for (const auto& target : member.targets)
        result["targets"].push_back(target);
    return result;
}

static json rollStackMemberToJson(const RollEventState& member) {
    json result;
    result["value"] = member.value;
    result["isCombatRoll"] = member.isCombatRoll;
    result["ownerID"] = member.ownerID;
    return result;
}

static json playerToJson(const PlayerBoardState& player) {
    json result;
    result["coinCount"] = player.coinCount;
    result["characterActive"] = player.characterActive;
    result["health"] = player.health;
    result["maxHealth"] = player.maxHealth;
    result["blueHealth"] = player.blueHealth;
    result["treasurePrice"] = player.treasurePrice;
    result["attack"] = player.attack;
    result["playableCount"] = player.playableCount;
    result["purchaseCount"] = player.purchaseCount;
    result["attackCount"] = player.attackCount;
    result["isDead"] = player.isDead;
    result["id"] = player.id;
    result["name"] = player.name;

    result["playerCard"] = cardToJson(player.playerCard);
    result["board"] = cardVectorToJson(player.board);
    result["hand"] = cardVectorToJson(player.hand);
    result["souls"] = cardVectorToJson(player.souls);
    return result;
}

json MatchState::toJson() {
    json result;
    result["boards"] = json::array();
    for (const auto& board : boards)
        result["boards"].push_back(playerToJson(board));
    result["stack"] = json::array();
    for (const auto& member : stack)
        result["stack"].push_back(stackMemberToJson(member));
    result["rollStack"] = json::array();
    for (const auto& member : rollStack)
        result["rollStack"].push_back(rollStackMemberToJson(member));
    result["monsterData"] = json::array();
    for (const auto& data : monsterDataArr)
        result["monsterData"].push_back(monsterDataToJson(data));

    result["turnCounter"] = turnCounter;
    result["currentI"] = currentI;
    result["priorityI"] = priorityI;
    result["currentID"] = currentID;
    result["isMain"] = isMain;
    result["lootDeckCount"] = lootDeckCount;
    result["treasureDeckCount"] = treasureDeckCount;
    result["monsterDeckCount"] = monsterDeckCount;
    result["lootDiscard"] = cardVectorToJson(lootDiscard);
    result["treasureDiscard"] = cardVectorToJson(treasureDiscard);
    result["monsterDiscard"] = cardVectorToJson(monsterDiscard);
    result["shop"] = cardVectorToJson(shop);
    result["monsters"] = cardVectorToJson(monsters);
    result["isCombat"] = isCombat;
    return result;
}

static Match* getTopMatch(lua_State* L, int pos) {
    if (!lua_isuserdata(L, pos)) {
        dumpstack(L);
        throw std::runtime_error("position at " + std::to_string(pos) + " of lua stack is not pointer to match");
    }
    return static_cast<Match*>(lua_touserdata(L, pos));
}

static string getTopString(lua_State* L, int pos) {
    if (!lua_isstring(L, pos)) {
        dumpstack(L);
        throw std::runtime_error("position at " + std::to_string(pos) + " of lua stack is not string");
    }
    return (string)lua_tostring(L, pos);
}

static int getTopNumber(lua_State* L, int pos) {
    if (!lua_isnumber(L, pos)) {
        dumpstack(L);
        throw std::runtime_error("position at " + std::to_string(pos) + " of lua stack is not a number");
    }
    return (int)lua_tonumber(L, pos);
}

static bool getTopBool(lua_State* L, int pos) {
    if (!lua_isboolean(L, pos)) {
        dumpstack(L);
        throw std::runtime_error("position at " + std::to_string(pos) + " of lua stack is not boolean");
    }
    return (bool)lua_toboolean(L, pos);
}

static void stackSizeIs(lua_State* L, int size) {
    if (lua_gettop(L) == size) return;
    throw std::runtime_error("expected lua stack size to be " + std::to_string(size) + ", but it is " + std::to_string(lua_gettop(L)));
}

static void pushCards(vector<CardWrapper*> cards, lua_State* L) {
    auto size = cards.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        cards[i]->pushTable(L);
        lua_settable(L, -3);
    }
}

StackEffect::StackEffect(string funcName, Player* player, CardWrapper* cardW, string type) :
    funcName(funcName),
    player(player),
    cardW(cardW),
    type(type) {
    }

StackEffect::StackEffect() {}

void StackEffect::pushTable(lua_State* L) {
    lua_newtable(L);
    l_pushtablestring(L, "type", type);
    if (player)
        l_pushtablenumber(L, "ownerID", player->id());
    if (cardW) {
        lua_pushstring(L, "card");
        cardW->pushTable(L);
        lua_settable(L, -3);
    }

    // string funcName;
    // Player* player;
    // CardWrapper* cardW;
    // string type;
    // bool resolve = true;
}

StackMemberState StackEffect::getState() {
    StackMemberState result;
    result.message = funcName;
    result.isCard = cardW;
    result.type = type;

    if (cardW) {
        result.card = cardW->getState();
        result.card.zone = Zones::Stack;
        result.card.active = true;
    }
    for (const auto targetID : targets)
        result.targets.push_back(targetID);
    return result;
}

void MatchState::pushTable(lua_State* L) const {
    lua_newtable(L);
    l_pushtableboolean(L, "isMain", isMain);
    l_pushtablenumber(L, "currentID", currentID);
    l_pushtableboolean(L, "inCombat", isCombat);
    l_pushtablenumber(L, "turnCounter", turnCounter);

    l_pushtablenumber(L, "lootDeckCount", lootDeckCount);
    l_pushtablenumber(L, "treasureDeckCount", treasureDeckCount);
    l_pushtablenumber(L, "monsterDeckCount", monsterDeckCount);

    lua_pushstring(L, "monsters");
    auto size = monsters.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        monsters[i].pushTable(L);
        lua_settable(L, -3);
    }
    lua_settable(L, -3);

    lua_pushstring(L, "monsterDataArr");
    size = monsterDataArr.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        monsterDataArr[i].pushTable(L);
        lua_settable(L, -3);
    }
    lua_settable(L, -3);

    lua_pushstring(L, "shop");
    size = shop.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        shop[i].pushTable(L);
        lua_settable(L, -3);
    }
    lua_settable(L, -3);

    lua_pushstring(L, "boards");
    size = boards.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        boards[i].pushTable(L);
        lua_settable(L, -3);
    }
    lua_settable(L, -3);



    //  TODO push loot discard
    //  TODO push monster discard
    //  TODO push treasure discard
}

Match::Match(nlohmann::json config, int seed) {
    _seed = seed;
    srand(seed);
    this->rng.seed(seed);
    this->_logWait = config.contains("logWait") ? (int)config["logWait"] : 0;
    this->_soulsToWin = config.contains("soulsToWin") ? (int)config["soulsToWin"] : 4;
    this->_perDeathCoins = config.contains("perDeathCoins") ? (int)config["perDeathCoins"] : 1;
    this->_perDeathLoot = config.contains("perDeathLoot") ? (int)config["perDeathLoot"] : 1;
    this->_perDeathItems = config.contains("perDeathItems") ? (int)config["perDeathItems"] : 1;
    this->_maxHandSize = config.contains("maxHandSize") ? (int)config["maxHandSize"] : 10;
    this->_startingLootAmount = config.contains("startingLootCount") ? (int)config["startingLootCount"] : 3;
    this->_startingCoinAmount = config.contains("startingCoinCount") ? (int)config["startingCoinCount"] : 3;
    this->_startingShopSize = config.contains("startingShopSize") ? (int)config["startingShopSize"]: 2;
    this->_startingMonstersAmount = config.contains("startingMonstersAmount") ? (int)config["startingMonstersAmount"]: 2;
    this->_startingTreasurePrice = config.contains("startingTreasurePrice") ? (int)config["startingTreasurePrice"] : 10;
    this->_startingAttackCount = config.contains("startingAttackCount") ? (int)config["startingAttackCount"] : 1;
    this->_startingPlayableCount = config.contains("startingPlayableCount") ? (int)config["startingPlayableCount"] : 1;
    this->_startingPurchaseCount = config.contains("startingPurchaseCount") ? (int)config["startingPurchaseCount"] : 1;

}

Match::Match(nlohmann::json config)
    : Match(config, time(0)) 
{}

Match::~Match() {
    // delete all players
    for (const auto& p : _players) delete p;
    // delete all wrappers
    for (const auto& w : _allWrappers) delete w;
    // close lua vm
    lua_close(this->L);
}

void printStack(const vector<StackEffect*>& stack) {
    std::cout << "\nSTACK:" << std::endl;
    for (int i = 0; i < stack.size(); i++)
        std::cout << "\t" << i << ") " << stack[i]->funcName << std::endl;
    std::cout << std::endl;
}

int Match::dealDamage(string tgtType, int tgtID, int amount) {
    if (tgtType == PLAYER_TYPE) {
        Player* target = this->playerWithID(tgtID);
        int dealt = target->dealDamage(amount);
        this->log(target->name() + " is dealt " + std::to_string(dealt) + " damage");
        if (!target->health()) pushDeathEvent(PLAYER_TYPE, target->id());
        return dealt;
    }
    if (tgtType == MONSTER_TYPE) {
        auto monsterW = this->cardWithID(tgtID);
        auto card = (MonsterCard*)monsterW->card();
        int pileI = -1;
        for (int i = 0; i < _monsters.size(); i++)
            if (_monsters[i].back() == monsterW)
                pileI = i;
        if (pileI == -1) return 0;
        auto data = _monsterDataArr[pileI];
        if (!data->health()) return 0;
        int dealt = data->dealDamage(amount);
        this->log(card->name() + " is dealt " + std::to_string(dealt) + " damage");
        auto health = data->health();
        if (health) return dealt;
        this->killMonster(monsterW);
        return dealt;
    }
    throw std::runtime_error("no damage was dealt, target: (" + tgtType + ", " + std::to_string(tgtID) + ")");
}

void Match::killMonster(CardWrapper* w) {
    if (_stack.back()->player)
        _lastKillerID = _stack.back()->player->id();
    // std::cout << "KILLING MONSTER " << std::endl;
    auto card = (MonsterCard*)w->card();
    card->data()->nullHealth();
    if (card->data()->isBeingAttacked()) {
        _lastMonsterIndex = -2;
        _isAttackPhase = false;
        applyTriggers(COMBAT_END_TYPE);
    }
    this->log(card->name() + " dies!");

    // rewards
    Effect& effect = card->rewardsEffect();
    effect.pushMe(this, w, _activePlayer, REWARDS_TYPE);

    // death
    effect = card->deathEffect();
    effect.pushMe(this, w, _activePlayer, MONSTER_DEATH_TRIGGER_TYPE);

    refillDeadMonsters();
}

void Match::addToLootDiscard(CardWrapper* wrapper) {
    this->_lootDiscard.push_back(wrapper);
    wrapper->setOwner(nullptr);
}

CardWrapper* Match::cardWithID(int id) {
    // std::cout << "wrappers: " << _allWrappers.size() << std::endl;
    for (const auto& w : _allWrappers) {
        // std::cout << w->card()->name() << " -> " << w->id() << std::endl;
        if (w->id() == id)
            return w;
    }
    throw std::runtime_error("no wrapper with id " + std::to_string(id));
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

CardWrapper* Match::getTopLootCard() {
    if (!_lootDeck.size()) this->shuffleLootDiscardIntoMain();
    if (_lootDeck.empty()) return nullptr;
    return _lootDeck.back();
}

vector<CardWrapper*> Match::getTopLootCards(int amount) {
    vector<CardWrapper*> result;
    while (amount) {
        auto card = getTopLootCard();
        if (!card) {
            this->log("The loot deck is empty");
            break;
        }
        result.push_back(card);
        _lootDeck.pop_back();
        amount--;
    }
    return result;
}

CardWrapper* Match::getTopTreasureCard() {
    if (!_treasureDeck.size()) this->shuffleTreasureDiscardIntoMain();
    if (_treasureDeck.empty()) return nullptr;
    return _treasureDeck.back();
}

CardWrapper* Match::getTopMonsterCard() {
    if (!_monsterDeck.size()) this->shuffleMonsterDiscardIntoMain();
    if (_monsterDeck.empty()) return nullptr;
    return _monsterDeck.back();
}

vector<CardWrapper*> Match::getTopTreasureCards(int amount) {
    vector<CardWrapper*> cards;
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

vector<CardWrapper*> Match::getTopMonsterCards(int amount) {
    vector<CardWrapper*> cards;
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

void Match::pushDamageEvent(DamageTrigger event) {
    _lastDamageEvent = event;
    this->applyTriggers(DAMAGE_TRIGGER);
}

void Match::pushPlayers(lua_State* L) {
    auto size = _players.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        _players[i]->pushTable(L);
        lua_settable(L, -3);
    }
}

bool Match::requestPayCost(string costFuncName, Player* player) {
    lua_getglobal(L, costFuncName.c_str());
    if (!lua_isfunction(L, -1)) {
        lua_err(L);
        throw std::runtime_error("failed to execute cost func " + costFuncName);
    }
    lua_pushlightuserdata(L, this);

    // push card info
    lua_newtable(L);
    l_pushtablenumber(L, "ownerID", (float)player->id());
    int r = lua_pcall(L, 2, 1, 0);
    if (r != LUA_OK) {
        lua_err(this->L);
        throw std::runtime_error("failed to call cost function " + costFuncName);
    }
    if (!lua_isboolean(L, -1)) {
        lua_err(this->L);
        throw std::runtime_error("cost func " + costFuncName + " didn't return a boolean");
    }
    return (bool)lua_toboolean(L, -1);

}

//  lua wrappers

int Match::wrap_popRewardsStack(lua_State* L) {
    stackSizeIs(L, 1);
    // auto match = getTopMatch(L, 1);
    // auto rewardsEvent = match->_rewardsStack.top();
    // match->_rewardsStack.pop();
    // auto cardW = rewardsEvent.monsterW;
    // auto effect = ((MonsterCard*)cardW->card())->rewardsEffect();
    // auto owner = match->_activePlayer;
    // effect->pushMe(match, cardW, );
    // lua_getglobal(L, funcName.c_str());
    // if (!lua_isfunction(L, -1)) {
    //     throw std::runtime_error("unknown function: " + funcName);
    // }
    // lua_pushlightuserdata(L, match);
    // rewardsEvent.monsterW->pushTable(L);
    // match->_activePlayer->pushTable(L);
    // // rewardsEvent.killer->pushTable(L);
    // int r = lua_pcall(L, 3, 0, 0);
    // if (r != LUA_OK) {
    //     throw std::runtime_error("failed to execute rewards function");
    // }
    return 0;
}

int Match::wrap_moveToHand(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto cid = getTopNumber(L, 3);
    auto player = match->playerWithID(pid);
    auto card = match->cardWithID(cid);
    player->addLootCards({card});
    return 0;
}

int Match::wrap_moveToBoard(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto cid = getTopNumber(L, 3);
    auto player = match->playerWithID(pid);
    auto card = match->cardWithID(cid);
    match->addCardToBoard(card, player);
    return 0;
}

int Match::wrap_setTurnEnd(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto value = getTopBool(L, 2);
    match->_turnEnd = value;
    return 0;
}

int Match::wrap_incPurchaseAmount(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto player = match->playerWithID(pid);
    player->incPurchaseAmount();
    return 0;
}

int Match::wrap_getTopOwner(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    auto last = match->_stack.back();
    last->player->pushTable(L);
    return 1;
}

int Match::wrap_removeFromEverywhere(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto cid = getTopNumber(L, 2);
    auto card = match->cardWithID(cid);
    
    // remove from all decks
    bool removed = false;
    removed = removeFromCollection(card, match->_treasureDeck);
    if (removed) return 0;
    removed = removeFromCollection(card, match->_monsterDeck);
    if (removed) return 0;
    removed = removeFromCollection(card, match->_lootDeck);
    if (removed) return 0;
    // remove from discards
    removed = removeFromCollection(card, match->_treasureDiscard);
    if (removed) return 0;
    removed = removeFromCollection(card, match->_monsterDiscard);
    if (removed) return 0;
    removed = removeFromCollection(card, match->_lootDiscard);
    if (removed) return 0;
    // remove from players
    for (const auto& player : match->_players) {
        removed = player->removeCard(card);
        if (removed) return 0;
    }
    // remove from shop
    removed = removeFromCollection(card, match->_shop);
    if (removed) return 0;
    // TODO remove from monster piles
    return 0;
}

int Match::wrap_getOwner(lua_State *L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto cid = getTopNumber(L, 2);
    auto w = match->cardWithID(cid);
    if (!w->owner()) {
        std::cout << "CARD " << w->card()->name() << " DOESN'T HAVE AN OWNER\n";
        // std::cout << "FIX ME ALREADY" << std::endl;
        lua_pushnil(L);
        return 1;
    }
    w->owner()->pushTable(L);
    return 1;
}

int Match::wrap_expandActiveMonsters(lua_State *L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto amount = getTopNumber(L, 2);
    auto w = match->getTopMonsterCard();
    if (!w) throw std::runtime_error("failed to create new monster pile: no monsters left");
    match->_monsterDeck.pop_back();
    auto monster = (MonsterCard*)w->card();
    monster->createData(match->L, match, w->id());
    vector<CardWrapper*> pile;
    pile.push_back(w);
    match->_monsters.push_back(pile);
    match->_monsterDataArr.push_back(monster->data());
    monster->enterEffect().pushMe(match, w, match->_activePlayer, MONSTER_ENTER_TYPE);
    return 0;
}

int Match::wrap_setRollValue(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto rid = getTopNumber(L, 2);
    auto value = getTopNumber(L, 3);
    int rc = 0;
    for (int i = 0; i < rid; i++) {
        if (match->_stack[i]->type != ROLL_TYPE) continue;
        rc++;
    }
    /*
    3

    0 play_loot
    1 buy_treasure
    2 activate
    3 roll
    4 activate

    0 roll
    */
    match->_rollStack[rc].value = value;
    // std::cout << "SIZE: " << match->_rollStack.size() << ", OPERATING: " << rc << std::endl;
    // std::cout << "NEW ROLL VALUE " << match->_rollStack[rc].value << std::endl;
    return 0;
}

int Match::wrap_addBlueHealth(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto amount  =getTopNumber(L, 3);
    Player* player = match->playerWithID(pid);
    player->addBlueHealth(amount);
    return 0;
}

int Match::wrap_dealDamage(lua_State* L) {
    stackSizeIs(L, 6);
    auto match = getTopMatch(L, 1);
    auto srcType = getTopString(L, 2);
    auto srcID = getTopNumber(L, 3);
    auto tgtType = getTopString(L, 4);
    auto tgtID = getTopNumber(L, 5);
    auto amount = getTopNumber(L, 6);

    int dealt = match->dealDamage(tgtType, tgtID, amount);
    if (!dealt) return 0;

    DamageTrigger trigger{
        srcType,
        srcID,

        tgtType,
        tgtID,

        amount,
        false,
        -1
    };
    match->pushDamageEvent(trigger);
    return 0;
}

int Match::wrap_getActivations(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    vector<StackEffect*> activations;
    for (const auto& effect : match->_stack)
        if (effect->type == ACTIVATE_ITEM_TYPE)
            activations.push_back(effect);
    auto size = activations.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        activations[i]->pushTable(L);
        lua_settable(L, -3);
    }
    return 1;
}

int Match::wrap_addCounters(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto cid = getTopNumber(L, 2);
    auto w = match->cardWithID(cid);
    auto amount = getTopNumber(L, 3);
    w->addCounters(amount);
    return 0;
}

int Match::wrap_removeCounters(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto cid = getTopNumber(L, 2);
    auto w = match->cardWithID(cid);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    w->removeCounters(amount);
    return 0;
}

int Match::wrap_pushTarget(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    int id = getTopNumber(L, 2);
    auto targetType = getTopString(L, 3);
    match->_targetStack.push_back(std::make_pair(targetType, id));

    match->_stack.back()->targets.push_back(id);
    return 0;
}

int Match::wrap_getDeathStack(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    auto size = match->_deathStack.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        match->_deathStack[i].pushTable(L);
        lua_settable(L, -3);
    }
    return 1;
}

int Match::wrap_getLastKillerID(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    lua_pushnumber(L, match->_lastKillerID);
    return 1;
}

int Match::wrap_expandShopSize(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    int amount = getTopNumber(L, 2);
    auto card = match->getTopTreasureCard();
    if (!card) throw std::runtime_error("shop is empty, can't expand shop size");
    match->_shop.push_back(card);
    match->_treasureDeck.pop_back();
    return 1;
}

int Match::wrap_getTurnCounter(lua_State* L){ 
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    lua_pushnumber(L, match->_turnCounter);
    return 1;
}

int Match::wrap_popTarget(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    auto pair = match->_targetStack.back();
    match->_targetStack.pop_back();
    lua_newtable(L);
    l_pushtablestring(L, "type", pair.first);
    l_pushtablenumber(L, "id", pair.second);
    return 1;
}

int Match::wrap_requestChoice(lua_State* L) {
    stackSizeIs(L, 5);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto text = getTopString(L, 3);
    auto choiceType = getTopString(L, 4);
    if(!lua_istable(L, 5)) {
        lua_err(L);
        exit(1);
    }
    vector<int> choices;
    auto size = lua_rawlen(L, 5);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, 5);
        choices.push_back(getTopNumber(L, -1));
        lua_pop(L, 1);
    }
    Player* player = match->playerWithID(pid);
    auto state = match->getState();
    auto response = player->promptResponse(state, text, choiceType, choices);
    // std::cout << "PROMPTING RESPONSE FOR TEXT " << text << std::endl;
    std::cout << "\t" << player->name() << ": " << response << " (response)" << std::endl;
    match->saveResponse(player->name(), response);
    if (response == RESPONSE_CANCEL) {
        lua_pushnumber(L, -1);
        lua_pushboolean(L, false);
        return 2;
    }
    int responseI = choices[0];
    if (response != RESPONSE_FIRST) {
        responseI = std::stoi(response.c_str());
    }
    lua_pushnumber(L, responseI);
    lua_pushboolean(L, true);
    return 2;
}

int Match::wrap_requestCardsInHand(lua_State* L){ 
    stackSizeIs(L, 5);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    Player* player = match->playerWithID(pid);
    auto tid = getTopNumber(L, 3);
    Player* target = match->playerWithID(tid); // just check that they exist
    auto text = getTopString(L, 4);
    auto amount = getTopNumber(L, 5);
    auto state = match->getState();
    auto result = player->promptChooseCardsInHand(state, text, tid, amount);
    match->saveResponse(player->name(), result);
    if (result == RESPONSE_FIRST) {
        auto hand = target->hand();
        result = std::to_string(hand[0]->id());
        for (int i = 1; i < amount; i++)
            result += " " + std::to_string(hand[i]->id());
    }
    auto split = str::split(result, " ");
    auto size = split.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        lua_pushnumber(L, atoi(split[i].c_str()));
        lua_settable(L, -3);
    }
    return 1;
    // requestCardsInHand(host, playerID, targetID, text, amount)
}

int Match::wrap_getAttack(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto player = match->playerWithID(pid);
    lua_pushnumber(L, player->baseAttack());
    return 1;
}

int Match::wrap_discardLoot(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    Player* player = match->playerWithID(pid);
    auto cid = getTopNumber(L, 3);
    auto card = player->takeCard(cid);
    match->_lootDiscard.push_back(card);
    return 0;
}

int Match::wrap_getMRoll(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto mid = getTopNumber(L, 2);
    MonsterData* mdata = nullptr;
    for (int i = 0; i < match->_monsters.size(); i++) {
        if (!match->_monsters[i].size()) continue;
        if (match->_monsters[i].back()->id() == mid) {
            mdata = match->_monsterDataArr[i];
            break;
        }
    }
    if (!mdata) throw std::runtime_error("failed to find active monter with id: " + std::to_string(mid));
    lua_pushnumber(L, mdata->baseRoll());
    return 1;
}

int Match::wrap_getMPower(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto mid = getTopNumber(L, 2);
    MonsterData* mdata = nullptr;
    for (int i = 0; i < match->_monsters.size(); i++) {
        if (!match->_monsters[i].size()) continue;
        if (match->_monsters[i].back()->id() == mid) {
            mdata = match->_monsterDataArr[i];
            break;
        }
    }
    if (!mdata) throw std::runtime_error("failed to find active monter with id: " + std::to_string(mid));
    lua_pushnumber(L, mdata->basePower());
    return 1;
}

int Match::wrap_requestSimpleChoice(lua_State* L) {
    if (lua_gettop(L) != 4) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    Player* player = match->playerWithID(pid);
    if (!lua_isstring(L, 3)) {
        lua_err(L);
        exit(1);
    }
    auto text = (string)lua_tostring(L, 3);
    if (!lua_istable(L, 4)) {
        lua_err(L);
        exit(1);
    }
    vector<string> choices;
    auto size = lua_rawlen(L, 4);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, 4);
        if (!lua_isstring(L, -1)) {
            lua_err(L);
            exit(1);
        }
        choices.push_back((string)lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    auto state = match->getState();
    auto response = player->promptSimpleResponse(state, text, choices);
    match->saveResponse(player->name(), response);
    if (response == RESPONSE_FIRST) response = choices[0];
    lua_pushstring(L, response.c_str());
    return 1;
}

int Match::wrap_getPlayers(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    match->pushPlayers(L);
    return 1;
}

int Match::wrap_setNextPlayer(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    // check if there is a player with id
    Player* player = match->playerWithID(pid);
    for (int i = 0; i < match->_players.size(); i++) {
        if (match->_players[i] == player) {
            match->_nextI = i;
            return 0;
        }
    }
    throw std::runtime_error("failed to set next player - can't find player with id " + std::to_string(pid));
}

//  TODO depricated
int Match::wrap_addSoulCard(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto cid = getTopNumber(L, 3);
    Player* player = match->playerWithID(pid);
    auto card = match->cardWithID(cid);
    player->addSoulCard(card);
    // std::cout << "ADDED SOUL CARD " << card->card()->name() << " TO PLAYER " << player->name() << std::endl;
    if (player->soulCount() >= match->_soulsToWin) {
        match->_winner = player;
        match->updateAllPlayersEndMatch();
    }
    return 0;
}

int Match::wrap_getTopDamageEvent(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    match->_lastDamageEvent.pushTable(L);
    return 1;
}

int Match::wrap_tapCharacterCard(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto player = match->playerWithID(pid);
    player->tapCharacter();
    return 0;
}

int Match::wrap_rechargeCharacterCard(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto player = match->playerWithID(pid);
    player->rechargeCharacter();
    return 0;
}

int Match::wrap_getDamageEvent(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    match->_lastDamageEvent.pushTable(L);
    return 1;
}

int Match::wrap_addCoins(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
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
    match->log(player->name() + " gains " + std::to_string(amount) + " coins");
    player->addCoins(amount);
    return 0;
}

int Match::wrap_getMonsterHealth(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto mid = getTopNumber(L, 2);
    for (int i = 0 ; i < match->_monsters.size(); i++) {
        if (!match->_monsters[i].size()) continue;
        if (match->_monsters[i].back()->id() == mid) {
            lua_pushnumber(L, match->_monsterDataArr[i]->health());
            return 1;
        }
    }
    throw std::runtime_error("no active monster with id: " + std::to_string(mid));
}

int Match::wrap_subCoins(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
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
    match->log(player->name() + " loses " + std::to_string(amount) + " coins");
    player->removeCoins(amount);
    return 0;
}

int Match::wrap_buyItem(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    CardWrapper* w = nullptr;
    auto top = match->getTopTreasureCard();
    if (top) {
        match->_treasureDeck.pop_back(); //  TODO fix if card is missing
    }
    int lti = match->_lastTreasureIndex;

    if (lti == -1) {
        w = top;
    } else {
        if (match->_shop.size() <= lti) throw std::runtime_error("shop index " + std::to_string(lti) + " is greater than shop size");

        w = match->_shop[lti];
        if (top) {match->_shop[lti] = top;}
        else {
            match->removeFromShop(w);
        }
    }

    auto player = match->_stack.back()->player;
    match->addCardToBoard(w, player);
    match->log("Player " + player->name() + " bought " + w->card()->name());
    // std::cout << "Player " + player->name() + " bought " + w->card()->name() << std::endl;;
    return 0;
}

int Match::wrap_lootCards(lua_State *L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);    
    auto amount = getTopNumber(L, 3);
    Player* player = match->playerWithID(pid);
    match->log(player->name() + " loots " + std::to_string(amount) + " cards");
    auto cards = match->getTopLootCards(amount);
    player->addLootCards(cards);
    return 0;
}

int Match::wrap_playTopLootCard(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    auto last = match->_stack.back();
    auto cardW = last->cardW;
    auto card = cardW->card();
    // if (!card->isTrinket()) {
    //     match->execFunc(card->useFuncName());
    //     if (card->goesToBottom())
    //         match->_lootDeck.push_front(cardW);
    //     else
    //         match->addToLootDiscard(cardW);
    // } else {
    //     match->addCardToBoard(cardW, last->player);
    // }
    return 0;
}

int Match::wrap_deferEOT(lua_State *L) {
    stackSizeIs(L, 4);
    auto match = getTopMatch(L, 1);
    auto cardID = getTopNumber(L, 2);
    auto funcName = getTopString(L, 3);
    auto isTrigger = getTopBool(L, 4);
    auto w = match->cardWithID(cardID);
    auto owner = w->owner();
    if (isTrigger) {
        auto effect = new StackEffect();;
        effect->funcName = funcName;
        effect->cardW = w;
        effect->player = owner;
        effect->type = TRIGGER_TYPE;
        match->_eotDeferredTriggers.push(effect);
    } else match->_eotDefers.push(funcName);
    return 0;
}

int Match::wrap_cancelCurrentAttack(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    match->_isAttackPhase = false;
    for (const auto& mdata : match->_monsterDataArr)
        mdata->setIsBeingAttacked(false);
    return 0;
}

int Match::wrap_healPlayer(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto amount = getTopNumber(L, 3);
    auto player = match->playerWithID(pid);
    player->heal(amount);
    return 0;
}

int Match::wrap_discardMe(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto cid = getTopNumber(L, 2);
    for (const auto& cardW : match->_allWrappers) {
        if (cardW->id() == cid) {
            match->sortToDiscard(cardW);
            return 0;
        }
    }
    throw std::runtime_error("can't discard card with unknown id " + std::to_string(cid));
}


int Match::wrap_this(lua_State *L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    match->_stack.back()->cardW->pushTable(match->L);
    return 1;
}

int Match::wrap_popRollStack(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    RollEvent& re = match->_rollStack.back();
    re.pushTable(L);
    match->_rollStack.pop_back();
    return 1;
}

int Match::wrap_dealCombatDamageF(lua_State* L) {
    stackSizeIs(L, 7);
    auto match = getTopMatch(L, 1);
    auto srcType = getTopString(L, 2);
    auto srcID = getTopNumber(L, 3);
    auto tgtType = getTopString(L, 4);
    auto tgtID = getTopNumber(L, 5);
    auto amount = getTopNumber(L, 6);
    auto rollValue = getTopNumber(L, 7);
    int dealt = match->dealDamage(tgtType, tgtID, amount);
    lua_pushnumber(L, dealt);
    return 1;
}

int Match::wrap_dealCombatDamage(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    auto& event = match->_lastCombatDamageEvent;
    //  TODO check if monster is source to deal damage
    if (match->_lastMonsterIndex == -2) return 0;
    if (match->_turnEnd) return 0;
    auto monsterData = match->_monsterDataArr[match->_lastMonsterIndex];
    if (!monsterData->isBeingAttacked()) return 0;

    lua_getglobal(L, "_dealCombatDamageF");
    if (!lua_isfunction(L, -1)) {
        lua_err(L);
        throw std::runtime_error("failed to fetch _dealCombatDamageF func");
    }
    lua_pushlightuserdata(L, match);
    lua_pushstring(L, event.sourceType.c_str());
    lua_pushnumber(L, event.sourceID);
    lua_pushstring(L, event.targetType.c_str());
    lua_pushnumber(L, event.targetID);
    lua_pushnumber(L, event.amount);
    lua_pushnumber(L, match->_lastCRollValue);
    int r = lua_pcall(L, 7, 1, 0);
    if (r != LUA_OK) {
        lua_err(match->L);
        throw std::runtime_error("failed to call check function _dealCombatDamageF");
    }

    int dealt = getTopNumber(L, -1);
    if (!dealt) return 0;
    if (match->_isAttackPhase) {
        match->_isAttackPhase = match->_activePlayer->health() && monsterData->health();
        if (!match->_isAttackPhase) match->_lastMonsterIndex = -2;
    }
    match->pushDamageEvent(event);
    return 0;
}

// int Match::wrap_popRollStack(lua_State* L) {
int Match::wrap_activateRoll(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    RollEvent& roll = match->_rollStack.back();
    match->_lastRoll = roll.value;
    match->_lastRollIsCombat = roll.isCombatRoll;
    match->_lastRollOwnerID = roll.owner->id();
    if (!roll.isCombatRoll) {
        match->pushToStack(roll.effect);
        roll.visible = false;
        match->applyTriggers(POST_ROLL_TYPE);
        return 0;
    }
    match->_lastCRollValue = roll.value;
    match->applyTriggers(POST_ROLL_TYPE);
    match->_rollStack.pop_back();
    if (match->_turnEnd) return 0;
    if (match->_lastMonsterIndex == -2) return 0;
    auto monsterW = match->_monsters[match->_lastMonsterIndex].back();
    auto monsterData = match->_monsterDataArr[match->_lastMonsterIndex];
    if (!monsterData->isBeingAttacked()) return 0;
    match->log( "Attack roll " + std::to_string(roll.value) + " vs " + std::to_string(monsterData->roll()));
    auto srcType = PLAYER_TYPE;
    auto srcID = match->_activePlayer->id();
    auto tgtType = MONSTER_TYPE;
    auto tgtID = monsterW->id();
    auto amount = match->_activePlayer->attack();
    if (roll.value < monsterData->roll()) {
        srcType = MONSTER_TYPE;
        srcID = monsterW->id();
        tgtType = PLAYER_TYPE;
        tgtID = match->_activePlayer->id();
        amount = monsterData->power();
    }
    DamageTrigger event{
        srcType,
        srcID,

        tgtType,
        tgtID,

        amount,
        true,
        -1
    };
    match->_lastCombatDamageEvent = event;
    match->pushToStack(new StackEffect(
        "_dealCombatDamage",
        match->_activePlayer, 
        nullptr,
        COMBAT_DAMAGE_TYPE
    ));
    // match->applyTriggers(ROLL_TYPE);
    return 0;
}

int Match::wrap_popDeathStack(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    auto event = match->_deathStack.back();
    // std::cout << "SETTING LAST DEATH TO " << event.type << "\t" << event.id << std::endl;
    match->_lastDeath = event;
    match->_deathStack.pop_back();
    if (event.type == PLAYER_TYPE) {
        match->killPlayer(event.id);
    }
    //  TODO? do something with the monster
    return 0;
}

int Match::wrap_getLastDeath(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    match->_lastDeath.pushTable(L);
    return 1;
}

int Match::wrap_destroyCard(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto cardID = getTopNumber(L, 2);
    auto card = match->cardWithID(cardID);
    for (const auto& player : match->_players) {
        for (const auto& bcard : player->board()) {
            if (bcard != card) continue;
            // destroy card
            //  TODO sort loot cards and treasure cards
            player->removeFromBoard(card);
            match->sortToDiscard(card);
            card->setOwner(nullptr);
            card->card()->leaveEffect().pushMe(match, card, player, ITEM_LEAVE_TYPE);
            return 0;
        }
    }
    // card is already gone
    match->log("WARNING: failed to remove non-existing card " + card->card()->name() + " from board");
    return 0;
}

int Match::wrap_incAdditionalCoins(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    Player* player = match->playerWithID(pid);
    player->incAdditionalCoins();
    return 0;
}

int Match::wrap_decAdditionalCoins(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    Player* player = match->playerWithID(pid);
    player->decAdditionalCoins();
    return 0;
}

static void millDeck(std::deque<CardWrapper*>* deck, std::deque<CardWrapper*>* discard, int amount) {
    while (amount) {
        if (deck->empty()) return;
        discard->push_back(deck->back());
        deck->pop_back();
        --amount;
    }
}

int Match::wrap_millDeck(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto deckType = getTopString(L, 2);
    int amount = getTopNumber(L, 3);
    auto deck = match->_deckMap[deckType];
    auto discard = match->_discardMap[deckType];
    millDeck(deck, discard, amount);
    return 0;
}

static void fromTopToBottom(std::deque<CardWrapper*>* deck, int amount) {
    while (amount) {
        if (deck->empty()) return;
        deck->push_front(deck->back());
        deck->pop_back();
        --amount;
    }
}

int Match::wrap_putFromTopToBottom(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto deckType = getTopString(L, 2);
    int amount = getTopNumber(L, 3);
    auto deck = match->_deckMap[deckType];
    fromTopToBottom(deck, amount);
    return 0;
}

int Match::wrap_getStack(lua_State* L) {
     if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);

    auto size = match->_stack.size();
    lua_createtable(L, size, 0);
    // std::cout 
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        match->_stack[i]->pushTable(L);
        lua_settable(L, -3);
    }
    return 1;
}

int Match::wrap_killEntity(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto type = getTopString(L, 2);
    auto id = getTopNumber(L, 3);
    if (type == PLAYER_TYPE) {
        match->pushDeathEvent(PLAYER_TYPE, id);
        return 0;
    }
    if (type == MONSTER_TYPE) {
        for (const auto& pile : match->_monsters) {
            // std::cout << "\t" << pile.back()->id() << "  " << pile.back()->card()->name() << std::endl;
            if (pile.back()->id() != id) continue;
            match->killMonster(pile.back());
            return 0;
        }
    }
    throw std::runtime_error("can't kill entity with id " + std::to_string(id) + " with unknown type " + type);
}

int Match::wrap_topCardsOf(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto deckType = getTopString(L, 2);
    int amount = getTopNumber(L, 3);
    auto deck = match->_deckMap[deckType];
    vector<CardWrapper*> result;
    auto size = deck->size();
    if (amount <= size) {
        for (int i = 0; i < amount; i++) {
            auto ii = size - i - 1;
            if (ii < 0) break;
            result.push_back(deck->at(ii));
        }
    }
    pushCards(result, L);
    return 1;
}

int Match::wrap_gainTreasure(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto amount = getTopNumber(L, 3);
    Player* player = match->playerWithID(pid);
    for (int i = 0; i < amount; i++) {
        auto top = match->getTopTreasureCard();
        if (!top) return 0;
        match->_treasureDeck.pop_back();
        match->addCardToBoard(top, player);
    }
    return 0;
}

int Match::wrap_addPlayableCount(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    Player* player = match->playerWithID(pid);
    player->incPlayableAmount();
    return 0;
}

int Match::wrap_incBeginningLoot(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
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
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int pid = (int)lua_tonumber(L, 2);
    auto player = match->playerWithID(pid);
    player->decBeginningLoot();
    return 0;
}

int Match::wrap_tapCard(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    auto cid = getTopNumber(L, 2);
    auto w = match->cardWithID(cid);
    w->tap();
    return 0;
}

int Match::wrap_rechargeCard(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    auto cid = getTopNumber(L, 2);
    auto w = match->cardWithID(cid);
    w->recharge();
    return 0;
}

int Match::wrap_getCurrentPlayer(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    match->_activePlayer->pushTable(L);
    return 1;
}

int Match::wrap_getMaxHealth(lua_State* L) {
    stackSizeIs(L, 2);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto player = match->playerWithID(pid);
    lua_pushnumber(L, player->baseMaxHealth());
    return 1;
}

int Match::wrap_incTreasureCost(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int pid = (int)lua_tonumber(L, 2);
    auto player = match->playerWithID(pid);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    player->incTreasureCost(amount);
    return 0;
}

int Match::wrap_decTreasureCost(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int pid = (int)lua_tonumber(L, 2);
    auto player = match->playerWithID(pid);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    player->decTreasureCost(amount);
    return 0;
}

int Match::wrap_incAttackCount(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int pid = (int)lua_tonumber(L, 2);
    auto player = match->playerWithID(pid);
    player->incAttackCount();
    return 0;
}

int Match::wrap_destroySoul(lua_State* L) {
    stackSizeIs(L, 3);
    auto match = getTopMatch(L, 1);
    auto pid = getTopNumber(L, 2);
    auto cid = getTopNumber(L, 3);
    auto player = match->playerWithID(pid);
    auto card = player->removeSoulCard(cid);
    match->sortToDiscard(card);
    return 0;
}

int Match::wrap_getRollStack(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    auto size = match->_rollStack.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        match->_rollStack[i].pushTable(L);
        lua_settable(L, -3);
    }
    return 1;
}

int Match::wrap_attackMonster(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = getTopMatch(L, 1);
    for (const auto& data : match->_monsterDataArr)
        if (data->isBeingAttacked())
            match->_isAttackPhase = true;            
    return 0;
}

int Match::wrap_getActiveMonsters(lua_State* L) {
    stackSizeIs(L, 1);
    auto match = getTopMatch(L, 1);
    auto size = match->_monsters.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        auto data = match->_monsterDataArr[i];
        lua_pushnumber(L, i+1);
        match->_monsters[i].back()->pushTable(L);
        l_pushtablenumber(L, "health", data->health());
        // these are unique, because they themselves use the lua stack
        lua_pushstring(L, "roll");
        data->roll();
        lua_settable(L, -3);
        lua_pushstring(L, "power");
        data->power();
        lua_settable(L, -3);
        
        l_pushtableboolean(L, "isBeingAttacked", data->isBeingAttacked());
        lua_settable(L, -3);
    }
    return 1;
}

void Match::addCardToBoard(CardWrapper* w, Player* owner) {
    owner->addToBoard(w);
    w->card()->enterEffect().pushMe(this, w, owner, ITEM_ENTER_TYPE);
}

void Match::removeFromShop(CardWrapper* cardW) {
    for (auto it = _shop.begin(); it != _shop.end(); it++) {
        if (*it == cardW) {
            _shop.erase(it);
            log(cardW->card()->name() + " removed from shop");
            return;
        }
    }
    throw std::runtime_error("attempted to remove card with id " + cardW->card()->name() + " from shop(it isn't there)");
}

Player* Match::playerWithID(int id) {
    for (const auto& p : this->_players) {
        if (p->id() == id) 
            return p;
    }
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

// void Match::execEnter(CardWrapper* w, Player* owner) {
//     auto funcName = w->card()->enterFuncName();
//     if (!funcName.size()) return;

//     // this->execFunc(efn);
//     this->log("Executing enter function " + funcName);
//     lua_getglobal(L, funcName.c_str());
//     if (!lua_isfunction(L, -1)) {
//         lua_err(L);
//         throw std::runtime_error("failed to execute enter function " + funcName);
//     }
//     lua_pushlightuserdata(L, this);
//     w->pushTable(L);
//     owner->pushTable(L);
//     int r = lua_pcall(L, 3, 0, 0);
//     if (r != LUA_OK) {
//         lua_err(this->L);
//         throw std::runtime_error("failed to execute enter func of card " + w->card()->name());
//     }
// }

// void Match::execLeave(CardWrapper* w, Player* owner) {
//     auto funcName = w->card()->leaveFuncName();
//     if (funcName.size()) {
//         this->log("Executing leave function " + funcName);
//         lua_getglobal(L, funcName.c_str());
//         if (!lua_isfunction(L, -1)) {
//             lua_err(L);
//             throw std::runtime_error("failed to execute leave function " + funcName);
//         }
//         lua_pushlightuserdata(L, this);
//         w->pushTable(L);
//         owner->pushTable(L);
//         int r = lua_pcall(L, 3, 0, 0);
//         if (r != LUA_OK) {
//             lua_err(this->L);
//             throw std::runtime_error("failed to card function " + funcName);
//         }
//     }
//     w->resetCounters();
// }

void Match::sortToDiscard(CardWrapper* cardW) {
    switch (cardW->card()->type()) {
    case CardTypes::Loot:
        _lootDiscard.push_back(cardW);
        break;
    case CardTypes::Treasure:
        _treasureDiscard.push_back(cardW);
        break;
    case CardTypes::Monster:
        _monsterDiscard.push_back(cardW);
        break;
    default:
        std::cout << "WARN: Can't sort card " << cardW->card()->name() << " to discard pile\n";
        break;
    }
}

void Match::setupLua(string setupScript) {
    this->L = luaL_newstate();
    // connect common libs
    luaL_openlibs(L);
    // connect functions
    lua_register(L, "healPlayer", wrap_healPlayer);
    lua_register(L, "discardMe", wrap_discardMe);
    lua_register(L, "moveToHand", wrap_moveToHand);
    lua_register(L, "moveToBoard", wrap_moveToBoard);
    lua_register(L, "addSoulCard", wrap_addSoulCard);
    lua_register(L, "removeFromEverywhere", wrap_removeFromEverywhere);
    lua_register(L, "getActivations", wrap_getActivations);
    lua_register(L, "expandActiveMonsters", wrap_expandActiveMonsters);
    lua_register(L, "expandShopSize", wrap_expandShopSize);
    lua_register(L, "_getMRoll", wrap_getMRoll);
    lua_register(L, "_getMPower", wrap_getMPower);
    lua_register(L, "rechargeCharacterCard", wrap_rechargeCharacterCard);
    lua_register(L, "setTurnEnd", wrap_setTurnEnd);
    lua_register(L, "getTurnCounter", wrap_getTurnCounter);
    lua_register(L, "cancelCurrentAttack", wrap_cancelCurrentAttack);
    lua_register(L, "getLastKillerID", wrap_getLastKillerID);
    lua_register(L, "destroyCard", wrap_destroyCard);
    lua_register(L, "killEntity", wrap_killEntity);
    lua_register(L, "_popDeathStack", wrap_popDeathStack);
    lua_register(L, "getLastDeath", wrap_getLastDeath);
    lua_register(L, "_popRewardsStack", wrap_popRewardsStack);
    lua_register(L, "getOwner", wrap_getOwner);
    lua_register(L, "incAdditionalCoins", wrap_incAdditionalCoins);
    lua_register(L, "decAdditionalCoins", wrap_decAdditionalCoins);
    lua_register(L, "getRollStack", wrap_getRollStack);
    lua_register(L, "setRollValue", wrap_setRollValue);
    lua_register(L, "lootCards", wrap_lootCards);
    lua_register(L, "getMonsterHealth", wrap_getMonsterHealth);
    lua_register(L, "addCoins", wrap_addCoins);
    lua_register(L, "subCoins", wrap_subCoins);
    lua_register(L, "_buyItem", wrap_buyItem);
    lua_register(L, "deferEOT", wrap_deferEOT);
    lua_register(L, "_playTopLootCard", wrap_playTopLootCard);
    lua_register(L, "incBeginningLoot", wrap_incBeginningLoot);
    lua_register(L, "decBeginningLoot", wrap_decBeginningLoot);
    lua_register(L, "gainTreasure", wrap_gainTreasure);
    lua_register(L, "addPlayableCount", wrap_addPlayableCount);
    lua_register(L, "getCurrentPlayer", wrap_getCurrentPlayer);
    lua_register(L, "rechargeCard", wrap_rechargeCard);
    lua_register(L, "getDamageEvent", wrap_getDamageEvent);
    lua_register(L, "getTopDamageEvent", wrap_getTopDamageEvent);
    lua_register(L, "this", wrap_this);
    lua_register(L, "dealDamage", wrap_dealDamage);
    lua_register(L, "setNextPlayer", wrap_setNextPlayer);
    lua_register(L, "incTreasureCost", wrap_incTreasureCost);
    lua_register(L, "decTreasureCost", wrap_decTreasureCost);
    lua_register(L, "getPlayers", wrap_getPlayers);
    lua_register(L, "destroySoul", wrap_destroySoul);
    lua_register(L, "addBlueHealth", wrap_addBlueHealth);
    lua_register(L, "pushTarget", wrap_pushTarget);
    lua_register(L, "popTarget", wrap_popTarget);
    lua_register(L, "getDeathStack", wrap_getDeathStack);
    lua_register(L, "addCounters", wrap_addCounters);
    lua_register(L, "removeCounters", wrap_removeCounters);
    lua_register(L, "requestChoice", wrap_requestChoice);
    lua_register(L, "requestChoice", wrap_requestChoice);
    lua_register(L, "requestSimpleChoice", wrap_requestSimpleChoice);
    lua_register(L, "requestCardsInHand", wrap_requestCardsInHand);
    lua_register(L, "discardLoot", wrap_discardLoot);
    lua_register(L, "incAttackCount", wrap_incAttackCount);
    lua_register(L, "tapCharacterCard", wrap_tapCharacterCard);
    lua_register(L, "tapCard", wrap_tapCard);
    lua_register(L, "millDeck", wrap_millDeck);
    lua_register(L, "getTopOwner", wrap_getTopOwner);
    lua_register(L, "topCardsOf", wrap_topCardsOf);
    lua_register(L, "putFromTopToBottom", wrap_putFromTopToBottom);
    lua_register(L, "_dealCombatDamage", wrap_dealCombatDamage);
    lua_register(L, "_dealCombatDamageF", wrap_dealCombatDamageF);
    lua_register(L, "_activateRoll", wrap_activateRoll);
    lua_register(L, "popRollStack", wrap_popRollStack);
    lua_register(L, "getStack", wrap_getStack);
    lua_register(L, "_attackMonster", wrap_attackMonster);
    lua_register(L, "getActiveMonsters", wrap_getActiveMonsters);
    lua_register(L, "_getMaxHealth", wrap_getMaxHealth);
    lua_register(L, "_getAttack", wrap_getAttack);
    lua_register(L, "incPurchaseAmount", wrap_incPurchaseAmount);

    //  TODO add state checking after some functions

    // load card scripts
    for (const auto& w : _lootDeck) {
        // std::cout << "Loading script for " << w->card()->name() << std::endl;
        this->execScript(w->card()->script());
    }
    for (const auto& w : _treasureDeck) {
        // std::cout << "Loading script for " << w->card()->name() << std::endl;
        this->execScript(w->card()->script());
    }
    for (const auto& w : _monsterDeck) {
        // std::cout << "Loading script for " << w->card()->name() << std::endl;
        this->execScript(w->card()->script());
    }
    for (const auto& pair : _characterPool) {
        auto character = pair.first;
        // std::cout << "Loading script for " << character->name() << std::endl;
        this->execScript(character->script());
        auto card = character->startingItem();
        // std::cout << "Loading script for " << card->name() << std::endl;
        this->execScript(card->script());
    }
    std::cout << "Loading base script" << std::endl;
    // setup script
    this->execScript(""
    "LOOT_DECK = \'" + LOOT_DECK + "\'"
    "\nTREASURE_DECK = \'" + TREASURE_DECK + "\'"
    "\nMONSTER_DECK = \'" + MONSTER_DECK + "\'"
    "\nCARD = \'" + CARD_TARGET + "\'"
    "\nSTACK = \'" + STACK_MEMBER_TARGET + "\'"
    "\nPLAYER = \'" + PLAYER_TARGET + "\'"
    "\nMONSTER = \'" + MONSTER_TARGET + "\'"
    "\nSOUL = \'" + SOUL_TARGET + "\'"
    "\nROLL = \'" + ROLL_TYPE + "\'"
    "\nPOST_ROLL = \'" + POST_ROLL_TYPE + "\'"
    "\nPER_DEATH_LOOT = " + std::to_string(this->_perDeathLoot).c_str() + ""
    "\nPER_DEATH_COINS = " + std::to_string(this->_perDeathCoins).c_str() + ""
    "\nfunction _startTurnLoot(host)"
    "\n    local owner = getTopOwner(host)"
    "\n    lootCards(host, owner.id, owner.startTurnLootAmount)"
    "\nend"
    "\nmath.randomseed(" + std::to_string(_seed) + ")"
    "\nfunction _deathPenalty(host, player)"
    "\n    local ownerID = player.id"
    "\n    local function discard()"
    "\n        local amount = PER_DEATH_LOOT"
    "\n        amount = math.min(amount, #(player.hand))"
    "\n        if amount ~= 0 then"
    "\n            local message = 'Choose a card to discard'"
    "\n            if amount > 1 then"
    "\n                message = 'Choose '..amount..' cards to discard'"
    "\n            end"
    // "\n            for _, card in ipairs(player.hand) do"
    // "\n                print('CARD', card.id, card.name)"
    // "\n            end"
    "\n            local cardIDs = requestCardsInHand(host, ownerID, ownerID, message, amount)"
    "\n            for _, cid in ipairs(cardIDs) do"
    "\n                discardLoot(host, ownerID, cid)"
    "\n            end"
    "\n        end"    
    "\n    end"
    "\n    local function payCoins()"
    "\n        local amount = math.min(PER_DEATH_COINS, player.coins)"
    "\n        subCoins(host, player.id, amount)"
    "\n    end"
    "\n    local function destroyItems()"
    "\n        local cardIDs = {}"
    "\n        for _, card in ipairs(player['board']) do"
    "\n            if not card['isEternal'] then"
    "\n                cardIDs[#cardIDs+1] = card['id']"
    "\n            end"
    "\n        end"
    "\n        if #cardIDs == 1 then"
    "\n            destroyCard(host, cardIDs[1])"
    "\n            return"
    "\n        end"
    "\n        if #cardIDs == 0 then"
    "\n            return"
    "\n        end"
    "\n        local choice, payed = requestChoice(host, ownerID, 'Choose a card to destroy', CARD, cardIDs)"
    "\n        destroyCard(host, choice)"
    "\n    end"
    "\n    local function tapCards()"
    "\n        tapCharacterCard(host, player.id)"
    "\n        for _, card in ipairs(player.board) do"
    "\n            if not card.passive then"
    "\n                tapCard(host, card.id)"
    "\n            end"
    "\n        end"
    "\n    "
    "\n    end"
    "\n    discard()"
    "\n    payCoins()"
    "\n    destroyItems()"
    "\n    tapCards()"
    "\nend"
    "\nfunction _discardToHandSize(host, amount)"
    "\n    local player = getCurrentPlayer(host)"
    "\n    amount = #player.hand - amount"
    "\n    if amount <= 0 then return end"
    "\n    local cardIDs = requestCardsInHand(host, player.id, player.id, 'Discard to hand size ('..amount..')', amount)"
    "\n    for _, cid in ipairs(cardIDs) do"
    "\n        discardLoot(host, player.id, cid)"
    "\n    end"
    "\nend");
    std::cout << "Loading setup script" << std::endl;
    this->execScript(setupScript);
    std::cout << "All scripts loaded!" << std::endl;
}

void Match::lua_err(lua_State *L) {
    string errormsg = lua_tostring(L, -1);
    std::cout << "LUA ERR: " << errormsg << std::endl;
}

void Match::execScript(string script) {
    int r = luaL_dostring(L, script.c_str());
    if (r != LUA_OK) {
        std::cout<<script<<std::endl;
        lua_err(this->L);
        throw std::runtime_error("failed to load script");
    }
}

void Match::execFunc(string funcName) {
    this->log("Executing function " + funcName);
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1)) {
        throw std::runtime_error("unknown function: " + funcName);
    }
    lua_pushlightuserdata(L, this);
    int r = lua_pcall(L, 1, 0, 0);

    if (r != LUA_OK) {
        lua_err(this->L);
        throw std::runtime_error("failed to execute func " + funcName);
    }

    //  TODO run state checks
}

bool Match::execCheck(string funcName, CardWrapper* card) {
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1)) {
        lua_err(L);
        exit(1);
    }
    lua_pushlightuserdata(L, this);
    // push card table
    card->pushTable(L);

    int r = lua_pcall(L, 2, 1, 0);
    if (r != LUA_OK) {
        lua_err(this->L);
        throw std::runtime_error("failed to call check function " + funcName);
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

void Match::addPlayer(Player* player) {
    player->setLuaENV(L);
    player->setParent(this);
    player->addCoins(_startingCoinAmount);
    _players.push_back(player);
    player->setStartingValues( _startingTreasurePrice,  _startingAttackCount,  _startingPlayableCount,  _startingPurchaseCount);

    _allWrappers.push_back(player->characterCard());
    auto w = addWrapper(player->origCharacterCard()->startingItem());
    addCardToBoard(w, player);
    w->setOwner(player);
    w->tap();
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

CardWrapper* Match::addWrapper(ScriptCard* card) {
    auto id = newCardID();
    // std::cout << card->name() + " -> " + std::to_string(id) << std::endl;
    this->log(card->name() + " -> " + std::to_string(id), false);
    auto w = new CardWrapper(card, id);
    _allWrappers.push_back(w);
    return w;
}

void Match::createLootDeck(std::vector<std::pair<ScriptCard*, int>> pairs) {
    for (const auto& pair : pairs) {
        for (int i = 0; i < pair.second; i++) {
            auto w = addWrapper(pair.first);
            this->_lootDeck.push_back(w);
        }
    }
    this->shuffleLootDeck();
}

void Match::createTreasureDeck(std::vector<ScriptCard*> cards) {
    for (const auto& card : cards) {
        auto w = addWrapper(card);
        this->_treasureDeck.push_back(w);
    }
    this->shuffleTreasureDeck();
}

void Match::createMonsterDeck(std::vector<MonsterCard*> cards) {
    for (const auto& card : cards) {
        auto w = addWrapper(card);
        this->_monsterDeck.push_back(w);            
    }
    this->shuffleMonsterDeck();
}

void Match::start() {
    srand(_seed);
    _recordPath = fs::join("records", "record" + std::to_string(_seed) + ".fsr");
    this->_currentI = 0;

    // start the record
    _record = nlohmann::json::object();
    _record["seed"] = _seed;
    _record["first"] = this->_currentI;
    _record["actions"] = nlohmann::json::object();
    for (const auto player : _players)
        _record["actions"][player->name()] = nlohmann::json::array();
    std::cout << "\nThe game starts\n\n";
    // give starting hands
    for (auto& p : _players) {
        auto cards = this->getTopLootCards(_startingLootAmount);
        p->addLootCards(cards);
        this->log(p->name() + "\t" + std::to_string(p->id()));
    }
    // setup shop
    auto tcards = getTopTreasureCards(_startingShopSize);
    for (const auto& c : tcards)
        _shop.push_back(c);
    // setup monsters
    auto mcards = getTopMonsterCards(_startingMonstersAmount);
    for (const auto& c : mcards) {
        vector<CardWrapper*> pile;
        pile.push_back(c);
        _monsters.push_back(pile);
        auto mcard = ((MonsterCard*)c->card());
        mcard->createData(L, this, c->id());
        _monsterDataArr.push_back(mcard->data());
        mcard->enterEffect().pushMe(this, c, _activePlayer, MONSTER_ENTER_TYPE);
        // execMEnterLeave(c, mcard->enterFuncName());
    }

    this->_priorityI = this->_currentI;
    this->_running = true;

    while (!this->_winner) {
        this->calcNext();
        this->turn();
        this->passTurn();
    }
}

void Match::updateAllPlayersEndMatch() {
    for (auto& player : _players) {
        auto state = this->getState();
        player->updateEndMatch(state, _winner->id());
    }
}

void Match::passTurn() {
    this->_currentI = this->_nextI;
}

void Match::calcNext() {
    this->_nextI = (this->_currentI + 1) % _players.size();
}

void Match::currentPlayerLoot() {
    this->pushToStack(new StackEffect(
        "_startTurnLoot", 
        _activePlayer, 
        nullptr,
        LOOT_TYPE
    ));
}

void Match::turn() {
    this->log("TURN " + std::to_string(++this->_turnCounter) + " - ");
    this->_activePlayer = this->_players[this->_currentI];
    this->log(this->_activePlayer->name() + " starts their turn");

    this->_activePlayer->resetPlayableCount();
    this->_activePlayer->resetPurchaseCount();

    // recharge all of the items and character card of the active player
    this->_activePlayer->recharge();

    // apply all <start of turn> triggers, then resolve stack
    this->applyTriggers(TURN_START_TRIGGER);
    this->resolveStack();
    if (_winner) return;


    // add loot 1 to stack, then resolve stack
    this->currentPlayerLoot();
    this->resolveStack();
    if (_winner) return;

    // main step
    _isMainPhase = true;
    this->log(_activePlayer->name() + "'s main phase");
    string response = "";
    auto state = this->getState();
    while (!_turnEnd && (response != ACTION_PASS || _isAttackPhase)) {
        response = this->_activePlayer->promptAction(state);
        this->log("\t" + _activePlayer->name() + ": " + response);
        this->executePlayerAction(_activePlayer, response);
        if (_isAttackPhase && response == ACTION_PASS) {
            response = "__passAttack";
            log("Rolling for attack");
            this->rollAttack();
        }
        this->resolveStack();
        if (_winner) return;
        state = this->getState();
    }
    _isMainPhase = false;
    _isAttackPhase = false;
    this->log("End of " + this->_activePlayer->name() + "'s turn");

    // end of turn
    this->applyTriggers(TURN_END_TRIGGER);
    this->pushEOTDeferredTriggers();
    this->resolveStack();
    if (_winner) return;

    _activePlayer->setPlayableCount(0);
    _activePlayer->setPurchaseCount(0);

    // all silent end of turn effects
    this->execEOTDefers();
    if (_winner) return;
    this->resetEOT();
    _turnEnd = false;

    // force player to discard to hand size
    lua_getglobal(L, "_discardToHandSize");
    if (!lua_isfunction(L, -1)) {
        throw std::runtime_error("unknown function: _discardToHandSize");
    }

    lua_pushlightuserdata(L, this);
    lua_pushnumber(L, _maxHandSize);
    int r = lua_pcall(L, 2, 0, 0);
    if (r != LUA_OK) {
        lua_err(L);
        throw std::runtime_error("failed to call discard to hand size function");
    }

    for (const auto& data : _monsterDataArr)
        data->setIsBeingAttacked(false);
    // clear all stack (just in case)
    if (_rewardsStack.size()) {
        throw std::runtime_error("ERR: REWARDS STACK NOT EMPTY");
    }
    if (_rollStack.size()) {
        throw std::runtime_error("ERR: ROLL STACK NOT EMPTY");
    }
    if (_stack.size()) {
        throw std::runtime_error("ERR: STACK NOT EMPTY");
    }
    if (_eotDefers.size()) {
        throw std::runtime_error("ERR: EOT DEFERS STACK NOT EMPTY");
    }
    if (_eotDeferredTriggers.size()){
        throw std::runtime_error("ERR: EOT DEFERRED TRIGGERS STACK NOT EMPTY");
    }
    if (_deathStack.size()) {
        throw std::runtime_error("ERR: DEATH STACK NOT EMPTY");
    }
    if (_targetStack.size()) {
        throw std::runtime_error("ERR: TARGET STACK NOT EMPTY");
    }
}

void Match::dumpStacks() {
    using std::cout;
    using std::endl;

}

void Match::resetEOT() {
    for (auto& p : _players)
        p->resetEOT();
    healMonsters();
}

void Match::executePlayerAction(Player* player, string action) {
    this->saveResponse(player->name(), action);
    auto split = str::split(action, " ");
    if (!this->_actionMap.count(split[0])) throw std::runtime_error("don't have a handler for |" + action + "| action in match (player: " + player->name() + ")");
    this->_actionMap[split[0]](player, split);
}

void Match::queueTrigger(CardWrapper* wrapper, string triggerType, Player* owner, std::queue<QueuedTrigger>& out) {
    auto card = wrapper->card();
    if (!card->hasTrigger(triggerType)) return;
    this->log("Card " + card->name() + "[" + std::to_string(wrapper->id()) + "] has a " + triggerType + " trigger");
    auto effect = card->getTriggerWhen(triggerType);
    auto checkFuncName = effect.checkFuncName;
    if (!this->execCheck(checkFuncName, wrapper)) {
        this->log("Check failed");
        return;
    }
    this->log(card->name() + " is triggered");
    out.push(QueuedTrigger{
        effect,
        wrapper,
        owner
    });
}

void Match::applyTriggers(string triggerType) {
    int result = 0;
    std::queue<QueuedTrigger> queue;
    // monsters
    for(const auto& pile : _monsters) {
        auto w = pile.back();
        queueTrigger(w, triggerType, nullptr, queue);
    }
    // players
    for (int i = 0; i < _players.size(); i++) {
        auto player = _players[(_currentI + i) % _players.size()];
        auto board = player->board();
        for (const auto& w : board) {
            queueTrigger(w, triggerType, player, queue);
        }
    }
    // empty queue
    while (!queue.empty()) {
        auto qt = queue.front();
        auto& effect = qt.effect;
        queue.pop();
        effect.pushMe(this, qt.cardW, qt.owner, TRIGGER_TYPE);
    }
}

void Match::triggerLastEffectType() {
    auto effect = this->_stack.back();
    this->applyTriggers(effect->type);
}

void Match::pushToStack(StackEffect* effect) {
    this->_stack.push_back(effect);
}

void Match::resolveStack() {
    // while the stack isn't empty
    while (!this->_stack.empty()) {
        updateAllPlayers();
        this->resolveTop();
        if (_winner) break;
    }
}

void Match::updateAllPlayers() {
    auto state = this->getState();
    for (auto& player : _players) {
        player->update(state);
    }
}

void Match::resolveTop() {
    this->_priorityI = this->_currentI;
    const auto last = this->_currentI;
    auto effect = this->_stack.back();

    do {
        // prompt action
        auto response = this->promptPlayerWithPriority();
        auto player = _players[_priorityI];
        if (response == ACTION_PASS) {
            // player passes priority
            this->log(this->_players[this->_priorityI]->name() + " passes priority");
            saveResponse(player->name(), ACTION_PASS);
            this->_priorityI = (this->_priorityI + 1) % _players.size();
            continue;
        } else {
            this->executePlayerAction(player, response);
        }
        // player put something on stack
        return;
    } while (last != this->_priorityI);
    // resolve the ability
    this->execFunc(effect->funcName);
    // std::cout << "STACK SIZE: " << _stack.size() << std::endl;
    this->log("Popping " + effect->funcName + " from stack");
    // std::cout << "BEFORE ERASE" << std::endl;
    // printStack(_stack);
    _stack.erase(std::find(_stack.begin(), _stack.end(), effect));
    // std::cout << "STACK SIZE: " << _stack.size() << std::endl;
    // std::cout << "AFTER ERASE" << std::endl;
    // printStack(_stack);
    delete effect;
    updateAllPlayers();
}

string Match::promptPlayerWithPriority() {
    auto player = this->_players[this->_priorityI];
    // this->log("Player " + player->name() + " takes priority");
    auto state = this->getState();
    return player->promptAction(state);
}

void Match::log(string message, bool wait) {
    // return;
    std::cout << " - " << message << std::endl;
    if (wait) {
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        std::this_thread::sleep_for(std::chrono::milliseconds(this->_logWait));
    }
}

MatchState Match::getState() {
    MatchState result;
    for (const auto& p : _players) 
        result.boards.push_back(p->getState());
    int rsi = 0;
    int dsp = 0;

    // printStack(_stack);
    // for (const auto& re : _rollStack)
    //     std::cout << re.value << " -> " << re.visible << std::endl;
    for (auto& si : _stack){
        auto s = si->getState();
        if (si->type == ROLL_TYPE) {
            if (rsi >= _rollStack.size()) continue;
            auto roll = _rollStack[rsi];
            while (!roll.visible) {
                rsi++;
                if (rsi == _rollStack.size()) break;
                roll = _rollStack[rsi];
            }
            rsi++;
            if (!roll.visible) continue;
            s.message = "Roll: " + std::to_string(roll.value);
            if (roll.effect) {
                s.message += "\nfor\n " + roll.effect->cardW->card()->name();
            }
        }
        if (si->type == LOOT_TYPE) {
            s.message = "Looting";
        }
        if (si->type == BUY_TREASURE_TYPE) {
            string cardName = " top card";
            if (_lastTreasureIndex != -1) cardName = _shop[_lastTreasureIndex]->card()->name();
            s.message = "Buying\n" + cardName;
        }
        if (si->type == ATTACK_MONSTER_TYPE) {
            string monsterName = "top monster";
            if (_lastMonsterIndex != -1) {
                monsterName = " ???";
                if (_monsters.size() > _lastMonsterIndex) {
                    auto monsters = _monsters[_lastMonsterIndex];
                    if (monsters.size())
                        monsterName = monsters.back()->card()->name();
                }
            }
            s.message = "Attacking\n" + monsterName;
        }
        if (si->type == COMBAT_DAMAGE_TYPE) {
            s.message = "Combat\ndamage to\n";
            if (_lastCombatDamageEvent.targetType == PLAYER_TYPE) 
                s.message += _activePlayer->name();
            else {
                //  TODO fix
                if (_lastMonsterIndex != -2)
                    s.message += _monsters[_lastMonsterIndex].back()->card()->name();
                else s.message += "???";
            }
                
        }
        if (si->type == DEATH_TYPE) {
            string message = "Death of\n";
            DeathEvent* de = &_lastDeath;
            if (dsp < _deathStack.size()) {
                de = &_deathStack[dsp];
                ++dsp;
            }
            if (de->type == MONSTER_TYPE) {                
                message += cardWithID(de->id)->card()->name();
            } else {
                message += playerWithID(de->id)->name();
            }
            s.message = message;
        }
        result.stack.push_back(s);
    }

    result.turnCounter = _turnCounter;

    result.currentI = _currentI;

    if (_activePlayer) result.currentID = _activePlayer->id();
    
    result.priorityI = _priorityI;
    result.isMain = _isMainPhase && _stack.empty() && _priorityI == _currentI;

    result.lootDeckCount = _lootDeck.size();
    for (const auto& w : _lootDiscard) {
        auto s = w->getState();
        s.zone = Zones::LootDiscard;
        result.lootDiscard.push_back(s);
    }
    result.treasureDeckCount = _treasureDeck.size();
    for (const auto& w : _treasureDiscard) {
        auto s = w->getState();
        s.zone = Zones::TreasureDiscard;
        result.treasureDiscard.push_back(s);
    }
    for (const auto& w : _shop) {
        auto s = w->getState();
        s.zone = Zones::Shop;
        result.shop.push_back(s);
    }
    result.monsterDeckCount = _monsterDeck.size();
    for (const auto& w : _monsterDiscard) {
        auto s = w->getState();
        s.zone = Zones::MonsterDiscard;
        result.monsterDiscard.push_back(s);
    }
    auto size = _monsters.size();
    for (int i = 0; i < size; i++) {
        auto& pile = _monsters[i];
        auto& data = _monsterDataArr[i];
        if (!pile.size()) {
            auto s = emptyCardState();
            s.zone = Zones::ActiveMonsters;
            result.monsters.push_back(s);
            result.monsterDataArr.push_back(emptyMonsterDataState());
            continue;
        }
        auto s = pile.back()->getState();
        s.zone = Zones::ActiveMonsters;
        result.monsters.push_back(s);
        result.monsterDataArr.push_back(data->getState());
    }


    result.isCombat = _isAttackPhase;

    return result;
}

int Match::newCardID() {
    return ++_lastID;
}

void Match::rollAttack() {
    RollEvent p(
        _activePlayer,
        true
    );
    this->log(_activePlayer->name() + " rolls a " + std::to_string(p.value));
    this->_rollStack.push_back(p);
    this->pushToStack(new StackEffect(
        "_activateRoll",
        _activePlayer, 
        nullptr,
        ROLL_TYPE
    ));
    applyTriggers(ROLL_TYPE);
}

void Match::healMonsters() {
    for (const auto& data : _monsterDataArr)
        data->fullHeal();
}

void Match::refillDeadMonsters() {
    bool hasBonus = false;
    for (int i = 0; i < _monsters.size(); i++) {
        if (_monsterDataArr[i]->health()) continue;
        auto w = _monsters[i].back();
        auto mc = ((MonsterCard*)w->card());
        mc->deleteData();
        // std::cout << "EXECUTING MONSTER LEAVE " << w->card()->name() << " -- " << w->card()->leaveFuncName() << std::endl;
        mc->leaveEffect().pushMe(this, w, _activePlayer, MONSTER_LEAVE_TYPE);
        _monsters[i].pop_back();
        _monsterDataArr[i] = nullptr;
        if (w->card()->soulCount()) {
            _activePlayer->addSoulCard(w);
            if (_activePlayer->soulCount() >= _soulsToWin) {
                std::cout << "SETTING " << _activePlayer->name() << " TO WINNER" << std::endl;
                _winner = _activePlayer;
                updateAllPlayersEndMatch();
            }
        } else {
            _monsterDiscard.push_back(w);
        }
        CardWrapper* newM = nullptr;
        if (!_monsters[i].size()) {
            //  TODO? fix
            if (!_monsterDeck.size()) continue;
            newM = _monsterDeck.back();
            _monsterDeck.pop_back();
            _monsters[i].push_back(newM);
        } else {
            newM = _monsters[i].back();
        }
        auto mcard = ((MonsterCard*)newM->card());
        mcard->createData(L, this, newM->id());
        mcard->enterEffect().pushMe(this, newM, _activePlayer, MONSTER_ENTER_TYPE);
        _monsterDataArr[i] = mcard->data();
        pushDeathEvent(MONSTER_TYPE, w->id());
    }
    if (hasBonus) {
        //  TODO push function of bonus card
    }
}

// void Match::execMEnterLeave(CardWrapper* cardW, string funcName) {
//     if (!funcName.size()) return;

//     this->log("Executing monster function " + funcName);
//     lua_getglobal(L, funcName.c_str());
//     if (!lua_isfunction(L, -1)) {
//         lua_err(L);
//         throw std::runtime_error("failed to execute monster function " + funcName);
//     }
//     lua_pushlightuserdata(L, this);
//     cardW->pushTable(L);
//     int r = lua_pcall(L, 2, 0, 0);
//     if (r != LUA_OK) {
//         lua_err(this->L);
//         throw std::runtime_error("failed to execute monster func of card " + cardW->card()->name());
//     }
// }

void Match::pushDeathEvent(string type, int id) {
    _deathStack.push_back(DeathEvent{type, id});
    pushToStack(new StackEffect(
        "_popDeathStack",
        _activePlayer, //  TODO ???
        nullptr,
        DEATH_TYPE
    ));
    this->triggerLastEffectType();
}

void Match::killPlayer(int id) {
    _lastKillerID = _stack.back()->player->id();
    auto player = playerWithID(id);
    if (player->isDead()) return;
    player->setIsDead(true);
    if (player == _activePlayer) _turnEnd = true;
    lua_getglobal(L, "_deathPenalty");
    if (!lua_isfunction(L, -1)) {
        throw std::runtime_error("unknown function: _deathPenalty");
    }
    lua_pushlightuserdata(L, this);
    player->pushTable(L);
    int r = lua_pcall(L, 2, 0, 0);
    if (r != LUA_OK) {
        lua_err(L);
        throw std::runtime_error("failed to call death penalty function");
    }
    this->applyTriggers(AFTER_DEATH_TYPE);
}

void Match::saveResponse(string playerName, string response) {
    _record["actions"][playerName].push_back(response);
    std::ofstream out(_recordPath);
    out << _record.dump(4);
    out.close();
}

void Match::saveResponse(string playerName, int response) {
    _record["actions"][playerName].push_back(std::to_string(response));
    std::ofstream out(_recordPath);
    out << _record.dump(4);
    out.close();
}