#include "cards.hpp"

#include "match.hpp"
#include "player.hpp"

Card::Card(string dir, json j) : 
    _name(j["name"]),
    _text(j["text"])
{}

string Card::name() { return _name; }
string Card::text() { return _text; }

void Card::print(string prefix) {
    std::cout << prefix << _name << std::endl;
}

ScriptCard::ScriptCard(string dir, json j, bool isTrinket, bool isEternal) : 
    Card(dir, j),
    _isTrinket(isTrinket),
    _isEternal(isEternal)

{ 
    this->_script = fs::readFile(fs::join(dir, j["script"]).c_str());
    if (_isTrinket) {
        if (j.contains("on")) {
            auto triggers = j["on"];
            for (const auto& pair : triggers.items()) {
                auto v = pair.value();
                Trigger trigger;
                trigger.checkFuncName = v["check"];
                trigger.effectFuncName = v["effect"];
                if (v.contains("usesStack")) trigger.usesStack = v["usesStack"];
                if (v.contains("cost")) trigger.costFuncName = v["cost"];
                _triggerMap[pair.key()] = trigger;
            }
        }
        if (j.contains("enter"))
            this->_enterFuncName = j["enter"];
        if (j.contains("leave"))
            this->_leaveFuncName = j["leave"];
        if (j.contains("abilities")) {
            for (const auto& jj : j["abilities"].items()) {
                auto a = jj.value();
                ActivatedAbility ability;
                ability.funcName = a["ability"];
                ability.costFuncName = a["cost"];
                _abilities.push_back(ability);
            }
        }
        if (j.contains("eternal"))
            _isEternal = j["eternal"];
    } else {
        this->_useFuncName = j["use"];
        if (j.contains("goes_to_bottom"))
            this->_goesToBottom = j["goes_to_bottom"];
        if (j.contains("cost"))
            this->_costFuncName = j["cost"];
    }
}

ScriptCard::~ScriptCard() {

}

string ScriptCard::script() { return _script; }

std::vector<ActivatedAbility> ScriptCard::abilities() { return _abilities; }
bool ScriptCard::hasTrigger(string triggerName) { return _triggerMap.count(triggerName); }
Trigger ScriptCard::getTriggerWhen(string triggerName) { return _triggerMap[triggerName]; }

string ScriptCard::useFuncName() { return _useFuncName; }
string ScriptCard::enterFuncName() { return _enterFuncName; }
string ScriptCard::leaveFuncName() { return _leaveFuncName; }

string ScriptCard::costFuncName() { return _costFuncName; }
bool ScriptCard::isTrinket() { return _isTrinket; }
bool ScriptCard::isEternal() { return _isEternal; }
bool ScriptCard::goesToBottom() { return _goesToBottom; }

CharacterCard::CharacterCard(string dir, json j) : 
    ScriptCard(dir, j, false),
    _attack(j["attack"]),
    _health(j["health"])
{
    auto itemDir = fs::join(dir, j["item"]);
    auto jj = fs::readJS(fs::join(itemDir, CARD_INFO_FILE));
    this->_startingItem = new ScriptCard(itemDir, jj, true, true);
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
    ScriptCard(dir, j, true)
{
    _rewardsFuncName = j["rewards"];
    _baseHealth = j["health"];
    _baseRoll = j["roll"];
    _basePower = j["power"];
    if (j.contains("canBeAttacked"))
        _canBeAttacked = j["canBeAttacked"];
    // std::cout << "MONSTER " << name() << "\t" << _baseHealth << " " << _baseRoll << " " << _basePower << std::endl;
}

MonsterCard::~MonsterCard() {
    delete _data;
}

string MonsterCard::rewardsFuncName() { return _rewardsFuncName; }

MonsterData* MonsterCard::data() { return _data; }
bool MonsterData::isBeingAttacked() { return _isBeingAttacked; }
void MonsterData::setIsBeingAttacked(bool value) { _isBeingAttacked = value; }

void MonsterCard::deleteData() {
    delete _data;
}

void MonsterCard::createData(lua_State* L, Match* parent, int id) {
    _data = new MonsterData(L, parent, id, _baseHealth, _baseRoll, _basePower, _canBeAttacked);
}

CardWrapper::CardWrapper(ScriptCard* card, int id) :
    _card(card),
    _id(id) {}

ScriptCard* CardWrapper::card() { return _card; }
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
    result.cardName = _card->name();
    result.active = !_tapped;
    result.id = _id;
    result.counters = _counters;
    result.zone = Zones::Unknown;
    result.ownerID = -1;
    if (_owner) result.ownerID = _owner->id();
    result.activatedAbilityCount = _card->abilities().size();
    return result;
}

void CardWrapper::addCounters(int amount) { _counters += amount; }
void CardWrapper::removeCounters(int amount) { _counters -= amount; }