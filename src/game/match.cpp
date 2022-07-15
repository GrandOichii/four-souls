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
    result.soulCount = j["soulCount"];
    result.attack = j["attack"];
    result.health = j["health"];
    result.playableCount = j["playableCount"];
    result.purchaseCount = j["purchaseCount"];

    result.playerCard = cardFromJson(j["playerCard"]);
    result.board = cardVectorFromJson(j["board"]);
    result.hand = cardVectorFromJson(j["hand"]);
    return result;
}

static StackMemberState stackMemberFromJson(json j) {
    StackMemberState result;
    result.message = j["message"];
    result.isCard = j["isCard"];
    result.card = cardFromJson(j["card"]);
    return result;
}

MatchState::MatchState(nlohmann::json j){
    // boards = vector<PlayerBoardState>();
    for (const auto& [key, value] : j["boards"].items())
        boards.push_back(playerFromJson(value));
    // stack = vector<StackMemberState>();
    for (const auto& [key, value] : j["stack"].items())
        stack.push_back(stackMemberFromJson(value));
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
}

static json cardToJson(const CardState& card) {
    json result;
    result["cardName"] = card.cardName;
    result["active"] = card.active;
    result["id"] = card.id;
    result["counters"] = card.counters;
    result["zone"] = card.zone;
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
    result["card"] = cardToJson(member.card);
    return result;
}

static json playerToJson(const PlayerBoardState& player) {
    json result;
    result["coinCount"] = player.coinCount;
    result["characterActive"] = player.characterActive;
    result["health"] = player.health;
    result["maxHealth"] = player.maxHealth;
    result["blueHealth"] = player.blueHealth;
    result["soulCount"] = player.soulCount;
    result["treasurePrice"] = player.treasurePrice;
    result["attack"] = player.attack;
    result["playableCount"] = player.playableCount;
    result["purchaseCount"] = player.purchaseCount;

    result["playerCard"] = cardToJson(player.playerCard);
    result["board"] = cardVectorToJson(player.board);
    result["hand"] = cardVectorToJson(player.hand);
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
    return result;
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
    type(type) {}

StackEffect::StackEffect() {}

StackMemberState StackEffect::getState() {
    StackMemberState result;
    result.message = funcName;
    result.isCard = cardW;
    if (cardW) {
        result.card = cardW->getState();
        result.card.zone = Zones::Stack;
        result.card.active = true;
    }
    return result;
}

void MatchState::pushTable(lua_State* L) const {
    lua_newtable(L);
    l_pushtableboolean(L, "isMain", isMain);
    l_pushtablenumber(L, "currentID", currentID);
}

Match::Match() {
    this->rng.seed(rand());
}

Match::~Match() {
    // delete all players
    for (const auto& p : _players) delete p;
    // delete all wrappers
    for (const auto& w : _allWrappers) delete w;
    // close lua vm
    lua_close(this->L);
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
        exit(1);
    }
    lua_pushlightuserdata(L, this);

    // push card info
    lua_newtable(L);
    l_pushtablenumber(L, "ownerID", (float)player->id());
    int r = lua_pcall(L, 2, 1, 0);
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

int Match::wrap_getTopOwner(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    auto last = match->_stack.back();
    last->player->pushTable(L);
    return 1;
}

int Match::wrap_getOwner(lua_State *L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto cid = (int)lua_tonumber(L, 2);
    auto w = match->cardWithID(cid);
    if (!w->owner()) std::cout << "FIX ME ALREADY" << std::endl;
    w->owner()->pushTable(L);
    return 1;
}

int Match::wrap_setRollValue(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto rid = (int)lua_tonumber(L, 2);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    auto value = (int)lua_tonumber(L, 3);
    // std::
    match->_rollStack[rid].value = value;
    std::cout << "NEW VALUE " << match->_rollStack[rid].value << std::endl;
    return 0;
}

int Match::wrap_addBlueHealth(lua_State* L) {
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
    player->addBlueHealth(amount);
    return 0;
}

int Match::wrap_dealDamage(lua_State* L) {
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
    int dealt = player->dealDamage(amount);
    if (!dealt) return 0;
    match->log(player->name() + " is dealt " + std::to_string(amount) + " damage");
    // trigger all "dealt damage" triggers
    DamageTrigger trigger{
        PLAYER_TARGET,
        pid,
        amount,
        -1
    };
    match->_damageStack.push(trigger);
    // trigger.shelf
    trigger.shelfLife = match->applyTriggers(DAMAGE_TRIGGER);
    if (!trigger.shelfLife) {
        std::cout << "No damage triggers, popping damage stack" << std::endl;
        match->_damageStack.pop();
    }
    return 0;
}

int Match::wrap_addCounters(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int cid = (int)lua_tonumber(L, 2);
    auto w = match->cardWithID(cid);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    w->addCounters(amount);
    return 0;
}

int Match::wrap_removeCounters(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int cid = (int)lua_tonumber(L, 2);
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
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto id = (int)lua_tonumber(L, 2);
    if (!lua_isstring(L, 3)) {
        lua_err(L);
        exit(1);
    }
    auto targetType = (string)lua_tostring(L, 3);
    match->_targetStack.push_back(std::make_pair(targetType, id));
    // std::cout << "TARGET TYPE " << targetType << "\t" << id << std::endl;
    return 0;
}

int Match::wrap_popTarget(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    auto pair = match->_targetStack.back();
    match->_targetStack.pop_back();
    lua_newtable(L);
    l_pushtablestring(L, "type", pair.first);
    l_pushtablenumber(L, "id", pair.second);
    return 1;
}

int Match::wrap_requestChoice(lua_State* L) {
    if (lua_gettop(L) != 5) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    if (!lua_isstring(L, 3)) {
        lua_err(L);
        exit(1);
    }
    auto text = (string)lua_tostring(L, 3);
    if (!lua_isstring(L, 4)) {
        lua_err(L);
        exit(1);
    }
    auto choiceType = (string)lua_tostring(L, 4);
    if(!lua_istable(L, 5)) {
        lua_err(L);
        exit(1);
    }
    vector<int> choices;
    auto size = lua_rawlen(L, 5);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i + 1);
        lua_gettable(L, 5);
        if (!lua_isnumber(L, -1)) {
            lua_err(L);
            exit(1);
        }
        choices.push_back((int)lua_tonumber(L, -1));
        lua_pop(L, 1);
    }
    Player* player = match->playerWithID(pid);
    std::cout << choices.size() << std::endl;
    auto state = match->getState();
    auto response = player->promptResponse(state, text, choiceType, choices);
    
    // clear lua stack ?
    std::cout << "\t" << player->name() << ": " << response << " (response)" << std::endl;
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
    if (lua_gettop(L) != 5) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    Player* player = match->playerWithID(pid);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    auto tid = (int)lua_tonumber(L, 3);
    Player* target = match->playerWithID(pid); // just check that they exist
    if (!lua_isstring(L, 4)) {
        lua_err(L);
        exit(1);
    }
    auto text = (string)lua_tostring(L, 4);
    if (!lua_isnumber(L, 5)) {
        lua_err(L);
        exit(1);
    }
    auto amount = (int)lua_tonumber(L, 5);
    auto state = match->getState();
    auto result = player->promptChooseCardsInHand(state, text, tid, amount);
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

int Match::wrap_requestSimpleChoice(lua_State* L) {
    if (lua_gettop(L) != 4) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
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
    std::cout << response << std::endl;
    if (response == RESPONSE_FIRST) response = choices[0];
    std::cout << response << std::endl;
    lua_pushstring(L, response.c_str());
    return 1;
}

int Match::wrap_getPlayers(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    match->pushPlayers(L);
    return 1;
}

int Match::wrap_setNextPlayer(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
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

int Match::wrap_addSouls(lua_State* L) {
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
    player->addSouls(amount);
    return 0;
}

int Match::wrap_getDamageEvent(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    auto event = match->_damageStack.top();
    event.pushTable(L);
    event.shelfLife--;
    if (!event.shelfLife) {
        std::cout << "Removing damage trigger" << std::endl;
        match->_damageStack.pop();
    }
    return 1;
}

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
    Player* player = match->playerWithID(pid);
    match->log(player->name() + " gains " + std::to_string(amount) + " coins");
    player->addCoins(amount);
    return 0;
}

int Match::wrap_subCoins(lua_State* L) {
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
    match->log(player->name() + " gains " + std::to_string(amount) + " coins");
    player->removeCoins(amount);
    return 0;
}

int Match::wrap_buyItem(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    CardWrapper* w = nullptr;
    auto top = match->getTopTreasureCard();
    std::cout << top->card()->name() << std::endl;
    if (top) match->_treasureDeck.pop_back(); //  TODO fix
    int lti = match->_lastTreasureIndex;

    if (lti == -1) {
        w = top;
    } else {
        if (match->_shop.size() <= lti) return 0; //  TODO replace this with error

        w = match->_shop[lti];
        if (top) {match->_shop[lti] = top;}
        else {
            match->removeFromShop(w);
        }
    }

    auto player = match->_stack.back()->player;
    match->addCardToBoard(w, player);
    match->log("Player " + player->name() + " bought " + w->card()->name());
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

int Match::wrap_playTopLootCard(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    auto last = match->_stack.back();
    auto cardW = last->cardW;
    auto card = cardW->card();
    if (!card->isTrinket()) {
        match->execFunc(card->useFuncName());
        if (card->goesToBottom())
            match->_lootDeck.push_front(cardW);
        else
            match->addToLootDiscard(cardW);
    } else {
        match->addCardToBoard(cardW, last->player);
    }
    return 0;
}

int Match::wrap_deferEOT(lua_State *L) {
    if (lua_gettop(L) != 4) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isstring(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto cardID = (int)lua_tonumber(L, 2);
    auto w = match->cardWithID(cardID);
    auto owner = w->owner();
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
        auto effect = new StackEffect();;
        effect->funcName = funcName;
        effect->cardW = w;
        effect->player = owner;
        effect->type = TRIGGER_TYPE;
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
    match->_stack.back()->cardW->pushTable(match->L);
    return 1;
}

int Match::wrap_pushRollEvent(lua_State* L) {
    // pushRollEvent(host, cardInfo["id"], false)
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    Player* player = match->playerWithID(pid);
    RollEvent p(
        player,
        false
    );
    match->log(player->name() + " rolls a " + std::to_string(p.value));
    match->_rollStack.push_back(p);
    match->pushToStack(new StackEffect(
        "_popRollStack",
        player, 
        nullptr,
        ROLL_TYPE
    ));
    match->applyTriggers(ROLL_TYPE);
    return 0;
}

int Match::wrap_popRollStack(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    match->_lastRoll = match->_rollStack.back().value;
    match->_lastRollOwnerID = match->_rollStack.back().owner->id();
    match->_rollStack.pop_back();
    return 0;
}

int Match::wrap_incAdditionalCoins(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
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
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    Player* player = match->playerWithID(pid);
    player->decAdditionalCoins();
    return 0;
}

int Match::wrap_getLastRoll(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    lua_newtable(L);
    l_pushtablenumber(L, "value", match->_lastRoll);
    l_pushtablenumber(L, "ownerID", match->_lastRollOwnerID);
    return 1;
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
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isstring(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto deckType = (string)lua_tostring(L, 2);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    auto deck = match->_deckMap[deckType];
    auto discard = match->_discardMap[deckType];
    // auto pair = match->_deckDiscardMap[deckType];
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
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isstring(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto deckType = (string)lua_tostring(L, 2);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    auto deck = match->_deckMap[deckType];
    fromTopToBottom(deck, amount);
    return 0;
}

int Match::wrap_topCardsOf(lua_State* L) {
    if (lua_gettop(L) != 3) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isstring(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto deckType = (string)lua_tostring(L, 2);
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    auto deck = match->_deckMap[deckType];
    vector<CardWrapper*> result;
    auto size = deck->size();
    for (int i = 0; i < amount; i++) {
        auto ii = size - i - 1;
        if (ii < 0) break;
        result.push_back(deck->at(ii));
    }
    pushCards(result, L);
    return 1;
}

int Match::wrap_plusOneTreasure(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    auto pid = (int)lua_tonumber(L, 2);
    Player* player = match->playerWithID(pid);
    auto top = match->getTopTreasureCard();
    if (!top) return 0;
    match->_treasureDeck.pop_back();
    match->addCardToBoard(top, player);
    return 0;
}

int Match::wrap_addPlayableCount(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
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

int Match::wrap_tempIncMaxLife(lua_State* L) {
    if (lua_gettop(L) != 3) {
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
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    player->tempIncMaxLife(amount);
    return 0;
}

int Match::wrap_tempIncAttack(lua_State* L) {
    if (lua_gettop(L) != 3) {
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
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    player->tempIncAttack(amount);
    return 0;
}

int Match::wrap_incMaxLife(lua_State* L) {
    if (lua_gettop(L) != 3) {
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
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    player->incMaxLife(amount);
    return 0;
}

int Match::wrap_tapCard(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int cid = (int)lua_tonumber(L, 2);
    auto w = match->cardWithID(cid);
    w->tap();
    return 0;
}

int Match::wrap_rechargeCard(lua_State* L) {
    if (lua_gettop(L) != 2) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    if (!lua_isnumber(L, 2)) {
        lua_err(L);
        exit(1);
    }
    int cid = (int)lua_tonumber(L, 2);
    auto w = match->cardWithID(cid);
    w->recharge();
    return 0;
}

int Match::wrap_decMaxLife(lua_State* L) {
    if (lua_gettop(L) != 3) {
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
    if (!lua_isnumber(L, 3)) {
        lua_err(L);
        exit(1);
    }
    int amount = (int)lua_tonumber(L, 3);
    player->decMaxLife(amount);
    return 0;
}

int Match::wrap_getCurrentPlayer(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
    match->_activePlayer->pushTable(L);
    return 1;
}

int Match::wrap_incTreasureCost(lua_State* L) {
    if (lua_gettop(L) != 3) {
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
    auto match = static_cast<Match*>(lua_touserdata(L, 1));
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
    //  TODO untested
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
    player->incAttackCount();
    return 0;
}

int Match::wrap_getRollStack(lua_State* L) {
    if (lua_gettop(L) != 1) {
        lua_err(L);
        exit(1);
    }
    auto _match = static_cast<Match*>(lua_touserdata(L, 1));
    auto size = _match->_rollStack.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        _match->_rollStack[i].pushTable(L);
        lua_settable(L, -3);
    }
    return 1;
}

void Match::addCardToBoard(CardWrapper* w, Player* owner) {
    owner->addToBoard(w);
    this->execEnter(w, owner);
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

void Match::execEnter(CardWrapper* w, Player* owner) {
    auto funcName = w->card()->enterFuncName();
    if (!funcName.size()) return;

    // this->execFunc(efn);
    this->log("Executing enter function " + funcName);
    lua_getglobal(L, funcName.c_str());
    if (!lua_isfunction(L, -1)) {
        lua_err(L);
        exit(1);
    }
    lua_pushlightuserdata(L, this);
    w->pushTable(L);
    owner->pushTable(L);
    int r = lua_pcall(L, 3, 0, 0);
    if (r != LUA_OK) {
        lua_err(this->L);
        exit(1);
    }
}

void Match::setupLua(string setupScript) {
    this->L = luaL_newstate();
    // connect common libs
    luaL_openlibs(L);
    // connect functions
    lua_register(L, "getOwner", wrap_getOwner);
    lua_register(L, "incAdditionalCoins", wrap_incAdditionalCoins);
    lua_register(L, "decAdditionalCoins", wrap_decAdditionalCoins);
    lua_register(L, "getRollStack", wrap_getRollStack);
    lua_register(L, "setRollValue", wrap_setRollValue);
    lua_register(L, "lootCards", wrap_lootCards);
    lua_register(L, "addCoins", wrap_addCoins);
    lua_register(L, "subCoins", wrap_subCoins);
    lua_register(L, "_buyItem", wrap_buyItem);
    lua_register(L, "deferEOT", wrap_deferEOT);
    lua_register(L, "_playTopLootCard", wrap_playTopLootCard);
    lua_register(L, "incBeginningLoot", wrap_incBeginningLoot);
    lua_register(L, "decBeginningLoot", wrap_decBeginningLoot);
    lua_register(L, "plusOneTreasure", wrap_plusOneTreasure);
    lua_register(L, "addPlayableCount", wrap_addPlayableCount);
    lua_register(L, "incMaxLife", wrap_incMaxLife);
    lua_register(L, "tempIncMaxLife", wrap_tempIncMaxLife);
    lua_register(L, "tempIncAttack", wrap_tempIncAttack);
    lua_register(L, "decMaxLife", wrap_decMaxLife);
    lua_register(L, "getCurrentPlayer", wrap_getCurrentPlayer);
    lua_register(L, "addSouls", wrap_addSouls);
    lua_register(L, "rechargeCard", wrap_rechargeCard);
    lua_register(L, "getDamageEvent", wrap_getDamageEvent);
    lua_register(L, "this", wrap_this);
    lua_register(L, "dealDamage", wrap_dealDamage);
    lua_register(L, "setNextPlayer", wrap_setNextPlayer);
    lua_register(L, "incTreasureCost", wrap_incTreasureCost);
    lua_register(L, "decTreasureCost", wrap_decTreasureCost);
    lua_register(L, "getPlayers", wrap_getPlayers);
    lua_register(L, "addBlueHealth", wrap_addBlueHealth);
    lua_register(L, "pushTarget", wrap_pushTarget);
    lua_register(L, "popTarget", wrap_popTarget);
    lua_register(L, "addCounters", wrap_addCounters);
    lua_register(L, "removeCounters", wrap_removeCounters);
    lua_register(L, "requestChoice", wrap_requestChoice);
    lua_register(L, "requestChoice", wrap_requestChoice);
    lua_register(L, "requestSimpleChoice", wrap_requestSimpleChoice);
    lua_register(L, "requestCardsInHand", wrap_requestCardsInHand);
    lua_register(L, "incAttackCount", wrap_incAttackCount);
    lua_register(L, "tapCard", wrap_tapCard);
    lua_register(L, "millDeck", wrap_millDeck);
    lua_register(L, "getTopOwner", wrap_getTopOwner);
    lua_register(L, "topCardsOf", wrap_topCardsOf);
    lua_register(L, "pushRollEvent", wrap_pushRollEvent);
    lua_register(L, "putFromTopToBottom", wrap_putFromTopToBottom);
    lua_register(L, "getLastRoll", wrap_getLastRoll);
    lua_register(L, "_popRollStack", wrap_popRollStack);

    //  TODO add state checking after some functions

    // load card scripts
    for (const auto& w : _lootDeck) {
        std::cout << "Loading script for " << w->card()->name() << std::endl;
        this->execScript(w->card()->script());
    }
    for (const auto& w : _treasureDeck) {
        std::cout << "Loading script for " << w->card()->name() << std::endl;
        this->execScript(w->card()->script());
    }
    for (const auto& pair : _characterPool) {
        auto character = pair.first;
        std::cout << "Loading script for " << character->name() << std::endl;
        this->execScript(character->script());
        auto card = character->startingItem();
        std::cout << "Loading script for " << card->name() << std::endl;
        this->execScript(card->script());
    }
    std::cout << "Loading setup script" << std::endl;
    this->execScript(setupScript);
    std::cout << "Loading base script" << std::endl;
    // setup script
    this->execScript("LOOT_DECK = \"" + LOOT_DECK + "\"\nTREASURE_DECK = \"" + TREASURE_DECK + "\"\nMONSTER_DECK = \"" + MONSTER_DECK + "\"\nCARD = \"" + CARD_TARGET + "\"\nROLL = \"" + ROLL_TARGET + "\"\nPLAYER = \"" + PLAYER_TARGET + "\"function _startTurnLoot(host)\n\tlocal owner = getTopOwner(host)\nlootCards(host, owner[\"id\"], owner[\"startTurnLootAmount\"])\nend");
    
    std::cout << "All scripts loaded!" << std::endl;
}

void Match::lua_err(lua_State *L) {
    string errormsg = lua_tostring(L, -1);
    std::cout << "LUA ERR:" << errormsg << std::endl;
}

void Match::execScript(string script) {
    int r = luaL_dostring(L, script.c_str());
    if (r != LUA_OK) {
        lua_err(this->L);
        exit(1);
    }
}

void Match::execFunc(string funcName) {
    this->log("Executing function " + funcName);
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

void Match::addPlayer(Player* player) {
    player->addCoins(STARTING_COIN_AMOUNT);
    _players.push_back(player);

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
    std::cout << "\nThe game starts\n\n";
    // give starting hands
    for (auto& p : _players) {
        auto cards = this->getTopLootCards(STARTING_LOOT_AMOUNT);
        p->addLootCards(cards);
        std::cout << p->name() << "\t" << p->id() << std::endl;
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

    int c = 30;
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

    // add loot 1 to stack, then resolve stack
    this->currentPlayerLoot();
    this->resolveStack();

    // main step
    _isMainPhase = true;
    this->log(_activePlayer->name() + "'s main phase");
    string response = "";
    auto state = this->getState();
    while ((response = this->_activePlayer->promptAction(state)) != ACTION_PASS) {
        std::cout << "\t" << _activePlayer->name() << ": " << response << std::endl;
        this->executePlayerAction(_activePlayer, response);
        this->resolveStack();
        state = this->getState();
    }
    _isMainPhase = false;
    this->log("End of " + this->_activePlayer->name() + "'s turn");

    // end of turn
    //  TODO prompt the player to choose the order of triggers
    this->applyTriggers(TURN_END_TRIGGER);
    this->pushEOTDeferredTriggers();
    this->resolveStack();

    _activePlayer->setPlayableCount(0);
    _activePlayer->setPurchaseCount(0);

    // all silent end of turn effects
    this->execEOTDefers();
    this->resetEOT();
}

void Match::resetEOT() {
    for (auto& p : _players)
        p->resetEOT();
}

void Match::executePlayerAction(Player* player, string action) {
    auto split = str::split(action, " ");
    // std::cout << "ACIONS"
    if (!this->_actionMap.count(split[0])) throw std::runtime_error("don't have a handler for |" + split[0] + "| action in match");
    this->_actionMap[split[0]](player, split);
}

int Match::applyTriggers(string triggerType) {
    int result = 0;
    // first the monsters

    // then the items, starting with the current player
    //  TODO prompt the player to order them
    for (int i = 0; i < _players.size(); i++) {
        auto player = _players[(_currentI + i) % _players.size()];
        auto board = player->board();
        for (const auto& w : board) {
            auto card = w->card();
            if (!card->hasTrigger(triggerType)) continue;
            this->log("Card " + card->name() + "[" + std::to_string(w->id()) + "] has a " + triggerType + " trigger");
            auto effect = card->getTriggerWhen(triggerType);
            auto checkFuncName = effect.checkFuncName;
            if (!this->execCheck(checkFuncName, w)) {
                std::cout << "Check failed" << std::endl;
                continue;
            }
            this->log(card->name() + " is triggered");
            auto p = new StackEffect(
                effect.effectFuncName, 
                player, 
                w,
                TRIGGER_TYPE    
            );
            this->pushToStack(p);
            if (effect.costFuncName.size()) {
                bool payed = this->requestPayCost(effect.costFuncName, player);
                if (!payed) {
                    this->_stack.pop_back();
                    delete p;
                    continue;
                }
            }
            ++result;
        }
    }
    return result;
}

void Match::triggerLastEffectType() {
    // auto effect = this->_lastStack;
    auto effect = this->_stack.back();

    this->applyTriggers(effect->type);
    // this->resolveStack();
}

void Match::pushToStack(StackEffect* effect) {
    this->_stack.push_back(effect);
    updateAllPlayers();
}

void Match::resolveStack() {
    // while the stack isn't empty
    while (!this->_stack.empty()) {
        this->resolveTop();
    }
}

void Match::updateAllPlayers() {
    for (auto& player : _players) {
        auto state = this->getState();
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
            this->_priorityI = (this->_priorityI + 1) % _players.size();
            continue;
        } else {
            this->executePlayerAction(player, response);
        }
        // player put something on stack
        return;
    } while (last != this->_priorityI);
    // resolve the ability
    // std::cout << "LAST STACK SET TO " << effect.funcName << std::endl;
    // this->_lastStack = effect;
    // std::cout << "STACK SIZE " << this->_stack;
    if (effect->resolve) this->execFunc(effect->funcName);
    _stack.erase(std::find(_stack.begin(), _stack.end(), effect));
    delete effect;
    updateAllPlayers();
}

string Match::promptPlayerWithPriority() {
    auto player = this->_players[this->_priorityI];
    this->log("Player " + player->name() + " takes priority");
    auto state = this->getState();
    return player->promptAction(state);
}

void Match::log(string message, bool wait) {
    std::cout << " - " << message << std::endl;
    if (wait) {
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

MatchState Match::getState() {
    MatchState result;
    for (const auto& p : _players) 
        result.boards.push_back(p->getState());
    int rsi = 0;
    for (auto& si : _stack){
        auto s = si->getState();
        if (si->type == ROLL_TYPE) {
            s.message = "Roll: " + std::to_string(_rollStack[rsi].value);
            rsi++;
        }
        if (si->type == LOOT_TYPE) {
            s.message = "Looting";
        }
        if (si->type == BUY_TREASURE_TYPE) {
            string cardName = " top card";
            if (_lastTreasureIndex != -1) cardName = _shop[_lastTreasureIndex]->card()->name();
            s.message = "Buying\n" + cardName;
        }
        result.stack.push_back(s);

        //  TODO DEATH_TYPE = "death";
    }
    result.currentI = _currentI;
    result.currentID = _activePlayer->id();
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
    for (const auto& w : _monsters) {
        auto s = w->getState();
        s.zone = Zones::ActiveMonsters;
        result.monsters.push_back(s);
    }
    return result;
}

int Match::newCardID() {
    return ++_lastID;
}