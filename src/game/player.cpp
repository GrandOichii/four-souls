#include "player.hpp"
#include "match.hpp"

Player::Player(std::string name, CharacterCard* card, int id) :
    _name(name),
    _id(id),
    _characterCard(card) 
{
    this->_maxHealth = card->health();
    this->_health = this->_maxHealth;

    this->_attack = card->attack();

    auto w = new CardWrapper(card->startingItem(), _id);
    w->tap();
    this->_board.push_back(w);
    this->_characterActive = false;
    this->_startTurnLootAmount = 1;

    this->_soulCount = 0;

    this->_coinCount = 0;
    this->_additionalCoins = 0;

    this->_blueHealth = 0;

    this->_treasurePrice = STARTING_TREASURE_PRICE;

    this->_maxAttackCount = STARTING_ATTACK_COUNT;
    this->_attackCount = this->_maxAttackCount;
}

Player::~Player()  {
    for (const auto& w : _board) delete w;
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

LootCard* Player::getCard(int cardI) {
    return _hand[cardI];
}

LootCard* Player::takeCard(int cardI) {
    return *_hand.erase(_hand.begin()+cardI-1);
}

std::vector<CardWrapper*> Player::board() { return _board; }
std::vector<LootCard*> Player::hand() { return _hand; }
bool Player::characterActive() { return _characterActive; }
string Player::name() { return _name; }
int Player::id() { return _id; }
int Player::soulCount() { return _soulCount; }
CharacterCard* Player::characterCard() { return _characterCard; }

void Player::rechargeCharacter() {
    this->_characterActive = true;
}

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
    // push basic stuff
    l_pushtablestring(L, "name", this->_name);
    l_pushtablenumber(L, "id", (float)this->_id);
    l_pushtablenumber(L, "startTurnLootAmount", (float)this->_startTurnLootAmount);
    l_pushtablenumber(L, "coins", (float)this->_coinCount);
    l_pushtablenumber(L, "souls", (float)this->_soulCount);
    // push cards in hand
    lua_pushstring(L, "hand");
    auto handSize = _hand.size();
    lua_createtable(L, handSize, 0);
    for (int i = 0; i < handSize; i++) {
        lua_pushnumber(L, i+1);
        lua_pushstring(L, _hand[i]->name().c_str());
        lua_settable(L, -3);
    }
    lua_settable(L, -3);
}

int Player::coinCount() { return _coinCount; }
void Player::addCoins(int amount) { this->_coinCount += amount + _additionalCoins; }
void Player::removeCoins(int amount) { this->_coinCount -= amount; }

void Player::payPricePerTreasure() {
    //  TODO
    this->_coinCount -= this->_treasurePrice;
}

void Player::incAdditionalCoins() { _additionalCoins++; }
void Player::decAdditionalCoins() { _additionalCoins--; }

void Player::addLootCards(vector<LootCard*> cards) {
    for (const auto& c : cards)
        this->_hand.push_back(c);
}

void Player::addToBoard(CardWrapper* w) {
    _board.push_back(w);
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
    result.playerCard.first = _characterCard->name();
    result.playerCard.second = _characterActive;
    result.coinCount = _coinCount;
    result.health = _health;
    result.maxHealth = maxHealth();
    result.blueHealth = _blueHealth;
    result.soulCount = _soulCount;
    result.attack = attack();
    for (const auto& w : _board)
        result.board.push_back(std::make_pair(w->card()->name(), w->isActive()));
    for (const auto& c : _hand)
        result.hand.push_back(c->name());
    return result;
} 

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


ScriptedPlayer::ScriptedPlayer(std::string name, CharacterCard* card, int id, string actions, string responses) :
    Player(name, card, id)
{
    auto split = str::split(actions, "\n");
    for (int i = split.size()-1; i >= 0; i--)
        _actions.push(split[i]);
    split = str::split(responses, "\n");
    for (int i = split.size()-1; i >= 0; i--)
        _responses.push(split[i]);
}

string ScriptedPlayer::promptAction() {
    if (_actions.empty()) return ACTION_PASS;
    auto result = _actions.top();
    _actions.pop();
    return result;
}

string ScriptedPlayer::promptResponse(string text, string choiceType, vector<int> choices) {
    if (_responses.empty()) return RESPONSE_CANCEL;
    auto result = _responses.top();
    _responses.pop();
    return result;
}