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

MonsterData::MonsterData(int health, int roll, int power) {
    _health = health;
    _maxHealth = _health;
    _blueHealth = 0;
    _baseRoll = roll;
    _basePower = power;
}

MonsterDataState MonsterData::getState() {
    MonsterDataState result;
    //  TODO
    result.health = _health;
    result.roll = _baseRoll;
    result.power = _basePower;
    result.blueHealth = _blueHealth;
    return result;
}

int MonsterData::roll() {
    //  TODO
    return _baseRoll;
}

int MonsterData::power() {
    //  TODO
    return _basePower;
}

bool MonsterData::health() {
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
    _data = new MonsterData(_baseHealth, _baseRoll, _basePower);
}

MonsterCard::~MonsterCard() {
    delete _data;
}

string MonsterCard::rewardsFuncName() { return _rewardsFuncName; }

MonsterData* MonsterCard::data() { return _data; }
bool MonsterData::isBeingAttacked() { return _isBeingAttacked; }
void MonsterData::setIsBeingAttacked(bool value) { _isBeingAttacked = value; }

void MonsterCard::resetData() {
    delete _data;
    _data = new MonsterData(_baseHealth, _baseRoll, _basePower);
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