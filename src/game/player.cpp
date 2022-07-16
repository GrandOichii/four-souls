#include "player.hpp"
#include "match.hpp"

Player::Player(std::string name, CharacterCard* card, int id) :
    _name(name),
    _id(id),
    _characterCardOrig(card)
{
    this->_maxHealth = card->health();
    this->_health = this->_maxHealth;

    this->_attack = card->attack();

    this->_characterCard = new CardWrapper(card, _id);

    this->_characterActive = false;
    this->_startTurnLootAmount = 1;

    this->_soulCount = 0;

    this->_coinCount = 0;
    this->_additionalCoins = 0;

    this->_blueHealth = 0;
}

Player::~Player()  {}

void Player::setStartingValues(int treasurePrice, int attackCount, int playableCount, int purchaseCount) {
    this->_treasurePrice = treasurePrice;

    this->_maxAttackCount = attackCount;
    this->_attackCount = this->_maxAttackCount;

    this->_maxPlayableCount = playableCount;
    this->_playableCount = 0;

    this->_maxPurchaseCount = purchaseCount;
    this->_purchaseCount = 0;
}

CharacterCard* Player::origCharacterCard() { return _characterCardOrig; }

int Player::dealDamage(int amount) {
    _blueHealth -= amount;
    if (_blueHealth >= 0) return 0;
    amount = -_blueHealth;
    _blueHealth = 0;
    _health -= amount;
    if (_health < 0) _health = 0;
    return amount;
}

void Player::incMaxAttackCount() {
    _maxAttackCount++;
    _attackCount++;
}

void Player::decMaxAttackCount() {
    _maxAttackCount--;
}

void Player::incAttackCount() {
    _attackCount++;
}

void Player::incBeginningLoot() { _startTurnLootAmount++; }
void Player::decBeginningLoot() { _startTurnLootAmount--; }

void Player::incTreasureCost(int amount) { this->_treasurePrice += amount; }
void Player::decTreasureCost(int amount) { this->_treasurePrice -= amount; }

void Player::addBlueHealth(int amount) { _blueHealth += amount; }
int Player::blueHealth() { return _blueHealth; }

void Player::print() {
    std::cout << _name << " (" << _id << ")" << std::endl;
    std::cout << "Board: " << std::endl;
    for (const auto& w : _board)
        std::cout << w->card()->name() << " (" << w->id() << ")" << std::endl;
}

CardWrapper* Player::takeCard(int cardID) {
    for (auto it = _hand.begin(); it != _hand.end(); it++) {
        if ((*it)->id() == cardID) {
            auto result = *it;
            _hand.erase(it);
            return result;
        }
    }
    throw std::runtime_error("player " + _name + " doesn't have a card with id " + std::to_string(cardID));
}

std::vector<CardWrapper*> Player::board() { return _board; }
std::vector<CardWrapper*> Player::hand() { return _hand; }
bool Player::characterActive() { return _characterActive; }
string Player::name() { return _name; }
int Player::id() { return _id; }
int Player::soulCount() { return _soulCount; }
CardWrapper* Player::characterCard() { return _characterCard; }

void Player::rechargeCharacter() {
    this->_characterActive = true;
}

void Player::tapCharacter() { _characterActive = false; }

void Player::rechargeCards() {
    for (auto& w : _board)
        w->recharge();
}

void Player::recharge() {
    this->rechargeCharacter();
    this->rechargeCards();
}

void Player::pushTable(lua_State* L) {
    lua_newtable(L);
    // push general info
    l_pushtablestring(L, "name", this->_name);
    l_pushtablenumber(L, "id", (float)this->_id);
    l_pushtablenumber(L, "startTurnLootAmount", (float)this->_startTurnLootAmount);
    l_pushtablenumber(L, "coins", (float)this->_coinCount);
    l_pushtablenumber(L, "souls", (float)this->_soulCount);
    l_pushtablenumber(L, "playableCount", (float)_playableCount);
    l_pushtablenumber(L, "purchaseCount", (float)_purchaseCount);
    l_pushtablenumber(L, "treasurePrice", _treasurePrice);
    l_pushtableboolean(L, "characterActive", _characterActive);
    // push cards in hand
    lua_pushstring(L, "hand");
    auto handSize = _hand.size();
    lua_createtable(L, handSize, 0);
    for (int i = 0; i < handSize; i++) {
        lua_pushnumber(L, i+1);
        _hand[i]->pushTable(L);
        lua_settable(L, -3);
    }
    lua_settable(L, -3);
    // push board

    lua_pushstring(L, "board");
    auto boardSize = _board.size();
    lua_createtable(L, boardSize, 0);

    for (int i = 0; i < boardSize; i++) {
        lua_pushnumber(L, i+1);
        _board[i]->pushTable(L);
        lua_settable(L, -3);
    }
    lua_settable(L, -3);
}

void Player::resetPlayableCount() {
    _playableCount = _maxPlayableCount;
}

void Player::setPlayableCount(int amount) {
    _playableCount = amount;
}

void Player::decPlayableAmount() { --_playableCount; }
void Player::incPlayableAmount() { ++_playableCount; }

void Player::resetPurchaseCount() {
    _purchaseCount = _maxPurchaseCount;
}

void Player::setPurchaseCount(int amount) {
    _purchaseCount = amount;
}

void Player::decPurchaseAmount() { --_purchaseCount; }
void Player::incPurchaseAmount() { --_purchaseCount; }

int Player::coinCount() { return _coinCount; }
void Player::addCoins(int amount) { this->_coinCount += amount + _additionalCoins; }
void Player::removeCoins(int amount) { this->_coinCount -= amount; }

void Player::payPricePerTreasure() {
    this->_coinCount -= this->_treasurePrice;
}

void Player::incAdditionalCoins() { _additionalCoins++; }
void Player::decAdditionalCoins() { _additionalCoins--; }

void Player::addLootCards(vector<CardWrapper*> cards) {
    for (const auto& c : cards) {
        this->_hand.push_back(c);
        c->setOwner(this);
    }
}

void Player::addToBoard(CardWrapper* w) {
    _board.push_back(w);
    w->setOwner(this);
}

void Player::incMaxLife(int amount) {
    _maxHealth += amount;
    _health += amount;
}

void Player::decMaxLife(int amount) {
    _maxHealth -= amount;
    if (_maxHealth < 0) throw std::runtime_error("max player health can't be less than or equal to 0");
    //  TODO consider decreasing actual health by that amount
}

int Player::health() { return _health; }
int Player::maxHealth() { return _maxHealth + _tempMaxHealthBoost; }

void Player::tempIncAttack(int amount) { _tempAttackBoost += amount; }
int Player::attack() { return _attack + _tempAttackBoost; }

PlayerBoardState Player::getState() {
    PlayerBoardState result;
    result.playerCard = _characterCard->getState();
    result.playerCard.zone = Zones::CCard;
    result.playerCard.active = _characterActive;
    result.coinCount = _coinCount;
    result.playableCount = _playableCount;
    result.purchaseCount = _purchaseCount;
    result.health = _health;
    result.treasurePrice = _treasurePrice;
    result.id = _id;
    result.name = _name;
    result.maxHealth = maxHealth();
    result.blueHealth = _blueHealth;
    result.soulCount = _soulCount;
    result.attack = attack();
    for (const auto& w : _board) {
        auto s = w->getState();
        s.zone = Zones::Board;
        result.board.push_back(s);
    }
    for (const auto& w : _hand) {
        auto s = w->getState();
        s.zone = Zones::Hand;
        result.hand.push_back(s);
    }
    return result;
}

void PlayerBoardState::pushTable(lua_State* L) {
    // lua_createtable(L);

}

// string toJson(const PlayerBoardState& state) {
//     string result = "{";
//     addValue(result, "coinCount", state.coinCount);
//     addValue(result, "characterActive", state.characterActive);
//     addValue(result, "health", state.health);
//     addValue(result, "maxHealth", state.maxHealth);
//     addValue(result, "blueHealth", state.blueHealth);
//     addValue(result, "soulCount", state.soulCount);
//     addValue(result, "attack", state.attack);
//     addValue(result, "playerCard", state.playerCard);
//     addValue(result, "board", state.board);
//     addValue(result, "hand", state.hand, true);
//     result += "}";
//     return result;
// }


void Player::addSouls(int amount) { _soulCount += amount; }

void Player::tempIncMaxLife(int amount) {
    _tempMaxHealthBoost += amount;
    _health += amount;
}

void Player::resetEOT() {
    _blueHealth = 0;
    _tempMaxHealthBoost = 0;
    _health = maxHealth();
    _tempAttackBoost = 0;
    _attackCount = _maxAttackCount;
}

BotPlayer::BotPlayer(std::string name, CharacterCard* card, int id, string script) :
    Player(name, card, id)
{
    // setup lua
    this->L = luaL_newstate();
    // connect common libs
    luaL_openlibs(L);
    int r = luaL_dostring(L, script.c_str());
    if (r != LUA_OK) throw std::runtime_error("failed to execute setup script for bot");
}

BotPlayer::~BotPlayer() {
    lua_close(L);
}

static const char* PROMPT_ACTION_FUNC = "Bot_PromptAction";
static const char* PROMPT_RESPONSE_FUNC = "Bot_PromptResponse";
static const char* PROMPT_SIMPLE_RESPONSE_FUNC = "Bot_PromptSimpleResponse";

string BotPlayer::promptAction(MatchState& state) {
    // if (_actions.empty()) return ACTION_PASS;
    // auto result = _actions.top();
    // if (result == "mainWait") {
    //     if (!isMyMain) return ACTION_PASS;
    //     _actions.pop();
    //     result = _actions.top();
    // }
    // _actions.pop();
    lua_getglobal(L, PROMPT_ACTION_FUNC);
    if (!lua_isfunction(L, -1)) throw std::runtime_error("bot doesn't have action prompt script");
    // lua_pushlightuserdata(L, this);
    this->pushTable(L);
    state.pushTable(L);
    int r = lua_pcall(L, 2, 1, 0);
    if (r != LUA_OK) throw std::runtime_error("bot action prompt script failed");
    if (!lua_isstring(L, -1)) throw std::runtime_error("bot action prompt didn't return a string");
    return (string)lua_tostring(L, -1);
}

void BotPlayer::update(MatchState& state) {
    
}

string BotPlayer::promptResponse(MatchState& state, string text, string choiceType, vector<int> choices) {
    //  TODO
    return "$FIRST";
}

string BotPlayer::promptSimpleResponse(MatchState& state, string text, vector<string> choices) {
    //  TODO
    return "$FIRST";
}

string BotPlayer::promptChooseCardsInHand(MatchState& state, string text, int targetID, int amount) { 
    //  TODO
    return "";
}
