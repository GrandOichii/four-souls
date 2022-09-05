#include "cards.hpp"

#include "match.hpp"
#include "player.hpp"


Card::Card(string dir, json j, CardTypes type) : 
    _name(j["name"]),
    _text(j["text"]),
    _type(type)
{
    if (j.contains("soulCount")) _soulCount = j["soulCount"];
}

string Card::name() { return _name; }
string Card::text() { return _text; }
int Card::soulCount() { return _soulCount; }
CardTypes Card::type() { return _type; }

void Card::print(string prefix) {
    std::cout << prefix << _name << std::endl;
}

ScriptCard::ScriptCard(string dir, json j, CardTypes type) : 
    Card(dir, j, type)
{ 
    this->_script = fs::readFile(fs::join(dir, j["script"]).c_str());
    if (j.contains("on")) {
        auto triggers = j["on"];
        for (const auto& pair : triggers.items()) {
            auto v = pair.value();
            _triggerMap[pair.key()] = Trigger(v);
        }
    }
    if (j.contains("enter"))
        this->_enterEffect = Effect(j["enter"]);
    if (j.contains("leave"))
        this->_leaveEffect = Effect(j["leave"]);
    if (j.contains("destroyed"))
        this->_destroyedEffect = Effect(j["destroyed"]);
    if (j.contains("abilities")) {
        for (const auto& jj : j["abilities"].items()) {
            auto a = jj.value();
            _abilities.push_back(Effect(a));
        }
    }
    if (j.contains("eternal"))
        this->_isEternal = j["eternal"];
    if (j.contains("use"))
        this->_useEffect = Effect(j["use"]);
    if (j.contains("alt")) {
        auto altj = j["alt"];
        string type = altj["type"];
        if (CARD_TYPE_MAP.count(type) == 0) throw std::runtime_error("unknown card type: " + type);
        auto ctype = CARD_TYPE_MAP.at(type);
        switch(ctype) {
        case CardTypes::Treasure:
            this->_alt = new ScriptCard(dir, altj["card"], CardTypes::Treasure);
            break;
        case CardTypes::Character:
            this->_alt = new CharacterCard(dir, altj["card"]);
            break;
        case CardTypes::Monster:
            this->_alt = new MonsterCard(dir, altj["card"]);
            break;
        default:
            throw std::runtime_error("can't create alt card for type " + type);
        }
    }
}

ScriptCard::~ScriptCard() {
    delete _alt;
}
bool ScriptCard::hasAlt() { return _alt; }
ScriptCard* ScriptCard::alt() { return _alt; }

string ScriptCard::script() { return _script; }

std::vector<Effect>& ScriptCard::abilities() { return _abilities; }
bool ScriptCard::hasTrigger(string triggerName) { return _triggerMap.count(triggerName); }
Trigger& ScriptCard::getTriggerWhen(string triggerName) { return _triggerMap[triggerName]; }

Effect& ScriptCard::useEffect() { return _useEffect; }
Effect& ScriptCard::enterEffect() { return _enterEffect; }
Effect& ScriptCard::leaveEffect() { return _leaveEffect; }
Effect& ScriptCard::destroyedEffect() { return _destroyedEffect; }

bool ScriptCard::isEternal() { return _isEternal; }

CharacterCard::CharacterCard(string dir, json j) : 
    ScriptCard(dir, j, CardTypes::Character),
    _attack(j["attack"]),
    _health(j["health"])
{
    if (j.contains("item")) {
        auto itemDir = fs::join(dir, j["item"]);
        auto jj = fs::readJS(fs::join(itemDir, CARD_INFO_FILE));
        this->_startingItem = new ScriptCard(itemDir, jj, CardTypes::StartingItem);
    }
}

CharacterCard::~CharacterCard() {
    delete _startingItem;
}

int CharacterCard::attack() { return _attack; }
int CharacterCard::health() { return _health; }
ScriptCard* CharacterCard::startingItem() { return _startingItem; }

MonsterData::MonsterData(lua_State* L, Match* parent, int mid, int health, int roll, int power, bool canBeAttacked) {
    _canBeAttacked = canBeAttacked;
    _L = L;
    _parent = parent;
    _mid = mid;
    _health = health;
    _maxHealth = _health;
    _blueHealth = 0;
    _baseRoll = roll;
    _basePower = power;
}

MonsterDataState MonsterData::getState() {
    MonsterDataState result;
    result.health = _health;
    result.roll = roll();
    result.power = power();
    result.canBeAttacked = _canBeAttacked;
    result.blueHealth = _blueHealth;
    return result;
}

MonsterDataState emptyMonsterDataState() {
    MonsterDataState result;
    result.health = -1;
    result.roll = -1;
    result.power = -1;
    return result;
}


int MonsterData::baseRoll() {
    return _baseRoll;
}

int MonsterData::roll() {
    lua_getglobal(_L, "_getMRoll");
    if (!lua_isfunction(_L, -1)) {
        throw std::runtime_error("unknown function: _getMRoll");
    }
    lua_pushlightuserdata(_L, _parent);
    lua_pushnumber(_L, _mid);
    int r = lua_pcall(_L, 2, 1, 0);
    if (r != LUA_OK) {
        dumpstack(_L);
        string errormsg = lua_tostring(_L, -1);
        std::cout << "LUA ERR: " << errormsg << std::endl;
        throw std::runtime_error("failed to call _getMRoll function");
    }
    if (!lua_isnumber(_L, -1)) {
        throw std::runtime_error("_getMRoll didn't return a number");
    }

    int result = (int)lua_tonumber(_L, -1);
    return result;
}

int MonsterData::basePower() {
    return _basePower;
}

int MonsterData::power() {
    lua_getglobal(_L, "_getMPower");
    if (!lua_isfunction(_L, -1)) {
        throw std::runtime_error("unknown function: _getMPower");
    }
    lua_pushlightuserdata(_L, _parent);
    lua_pushnumber(_L, _mid);

    int r = lua_pcall(_L, 2, 1, 0);
    if (r != LUA_OK) {
        dumpstack(_L);
        string errormsg = lua_tostring(_L, -1);
        std::cout << "LUA ERR: " << errormsg << std::endl;
        throw std::runtime_error("failed to call _getMPower function");
    }
    if (!lua_isnumber(_L, -1)) {
        throw std::runtime_error("_getMPower didn't return a number");
    }

    int result = (int)lua_tonumber(_L, -1);
    return result;
}

int MonsterData::baseHealth() {
    return _health;
}

int MonsterData::health() {
    return _health;
}

int MonsterData::maxHealth() {
    //  TODO
    return _maxHealth;
}

void MonsterData::fullHeal() {
    _health = maxHealth();
}

int MonsterData::dealDamage(int amount) {
    _blueHealth -= amount;
    if (_blueHealth >= 0) return 0;
    amount = -_blueHealth;
    _blueHealth = 0;
    _health -= amount;
    if (_health < 0) _health = 0;
    return amount;
}

bool MonsterData::canBeAttacked() { return _canBeAttacked; }

void MonsterData::nullHealth() {
    _health = 0;
}

MonsterCard::MonsterCard(string dir, json j) :
    ScriptCard(dir, j, CardTypes::Monster)
    // _rewardsEffect(Effect(j["rewards"])),
{
    _rewardsEffect = Effect(j["rewards"]);
    _baseHealth = j["health"];
    _baseRoll = j["roll"];
    _basePower = j["power"];
    if (j.contains("canBeAttacked"))
        _canBeAttacked = j["canBeAttacked"];
    if (j.contains("death"))
        _deathEffect = Trigger(j["death"]);
}

MonsterCard::~MonsterCard() {
    delete _data;
}

Effect& MonsterCard::rewardsEffect() { return _rewardsEffect; }
Trigger& MonsterCard::deathEffect() { return _deathEffect; }

MonsterData* MonsterCard::data() { return _data; }
bool MonsterData::isBeingAttacked() { return _isBeingAttacked; }
void MonsterData::setIsBeingAttacked(bool value) { _isBeingAttacked = value; }

void MonsterCard::deleteData() {
    delete _data;
}

void MonsterCard::createData(lua_State* L, Match* parent, int id) {
    // std::cout << "CREATING NEW DATA FOR " << name() << std::endl;
    _data = new MonsterData(L, parent, id, _baseHealth, _baseRoll, _basePower, _canBeAttacked);
}

CardWrapper::CardWrapper(ScriptCard* card, int id) :
    _card(card),
    _id(id) {
    }

ScriptCard* CardWrapper::card() { return _showAlt ? _card->alt() : _card; }
int CardWrapper::id() { return _id; }
void CardWrapper::recharge() { _tapped = false; }
void CardWrapper::tap() { _tapped = true; }
bool CardWrapper::isActive() { return !_tapped; }
Player* CardWrapper::owner() { return _owner; }
void CardWrapper::setOwner(Player* owner) { _owner = owner; }

void CardWrapper::pushTable(lua_State* L) {
    lua_newtable(L);
    l_pushtablestring(L, "name", _card->name().c_str());
    l_pushtablenumber(L, "id", (float)this->_id);
    l_pushtablenumber(L, "soulCount", (float)_card->soulCount());
    auto ownerID = -1;
    if (_owner) {
        ownerID = _owner->id();
    }
    l_pushtablenumber(L, "ownerID", (float)ownerID);
    l_pushtablenumber(L, "counters", (float)_counters);
    l_pushtableboolean(L, "tapped", _tapped);
    l_pushtableboolean(L, "passive", !_card->abilities().size());
    l_pushtableboolean(L, "isEternal", _card->isEternal());
}

CardState CardWrapper::getState() {
    CardState result;
    auto fCard = card();
    result.cardName = fCard->name();
    result.active = !_tapped;
    result.id = _id;
    result.soulCount = fCard->soulCount();
    result.counters = _counters;
    result.zone = Zones::Unknown;
    result.ownerID = -1;
    if (_owner) result.ownerID = _owner->id();
    for (const auto& ability : fCard->abilities())
        result.activatedAbilities.push_back(ability.getState());        
    return result;
}

CardState emptyCardState() {
    CardState result;
    result.cardName = "--no-card--";
    return result;
}

void CardState::pushTable(lua_State* L) const {
    lua_newtable(L);
    l_pushtablestring(L, "name", cardName);
}

void CardWrapper::addCounters(int amount) { _counters += amount; }
void CardWrapper::removeCounters(int amount) { _counters -= amount; }
void CardWrapper::resetCounters() { _counters = 0; }

void MonsterDataState::pushTable(lua_State* L) const {
    lua_newtable(L);
    l_pushtablenumber(L, "health", health);
    l_pushtablenumber(L, "roll", roll);
    l_pushtablenumber(L, "power", power);
    l_pushtablenumber(L, "blueHealth", blueHealth);
    l_pushtablenumber(L, "canBeAttacked", canBeAttacked);
}

void CardWrapper::flip() {
    if (!hasAlt()) throw std::runtime_error("tried to flip non-flippable card " + _card->name());
    _showAlt = !_showAlt;
}

bool CardWrapper::hasAlt() {
    return _showAlt || _card->hasAlt();
}