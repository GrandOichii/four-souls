#include "player.hpp"

Player::Player(std::string name, CharacterCard* card, int id) :
    _name(name),
    _id(id),
    _characterCard(card) 
{
    this->_baseMHealth = card->health();
    this->_health = this->_baseMHealth;

    this->_baseAttack = card->attack();
    auto w = new CardWrapper(card->startingItem(), _id);
    w->tap();
    this->_board.push_back(w);
    this->_characterActive = false;
    this->_startTurnLootAmount = 1;

    this->_soulCount = 0;

    this->_coinCount = 0;
    this->_additionalCoins = 0;
}

Player::~Player()  {
    for (const auto& w : _board) delete w;
}

void Player::incBeginningLoot() { _startTurnLootAmount++; }
void Player::decBeginningLoot() { _startTurnLootAmount--; }

void Player::print() {
    std::cout << _name << " (" << _id << ")" << std::endl;
    std::cout << "Board: " << std::endl;
    for (const auto& w : _board)
        std::cout << w->card()->name() << " (" << w->id() << ")" << std::endl;
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
    l_pushtablestring(L, "name", this->_name);
    l_pushtablenumber(L, "id", (float)this->_id);
    l_pushtablenumber(L, "startTurnLootAmount", (float)this->_startTurnLootAmount);
}

int Player::coinCount() { return _coinCount; }
void Player::addCoins(int amount) { this->_coinCount += amount + _additionalCoins; }
void Player::removeCoins(int amount) { this->_coinCount -= amount; }

void Player::payPricePerTreasure() {
    //  TODO
    this->_coinCount -= 10;
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

PlayerBoardState Player::getState() {
    PlayerBoardState result;
    result.playerCard.first = _characterCard->name();
    result.playerCard.second = _characterActive;
    result.coinCount = _coinCount;
    for (const auto& w : _board)
        result.board.push_back(std::make_pair(w->card()->name(), w->isActive()));
    for (const auto& c : _hand)
        result.hand.push_back(c->name());
    return result;
} 

ScriptedPlayer::ScriptedPlayer(std::string name, CharacterCard* card, int id, string actions) :
    Player(name, card, id)
{
    auto split = str::split(actions, "\n");
    for (int i = split.size()-1; i >= 0; i--)
        _actions.push(split[i]);
}

string ScriptedPlayer::promptAction() {
    if (_actions.empty()) return ACTION_PASS;
    auto result = _actions.top();
    _actions.pop();
    return result;
}