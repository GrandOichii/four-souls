#include "cards.hpp"

#include "player.hpp"
#include "match.hpp"

Card::Card(string dir, json j) : 
    _name(j["name"]),
    _text(j["text"]),
    _imagePath(fs::join(dir, j["image"])) { /*std::cout << _name << std::endl;*/ }

string Card::name() { return _name; }
string Card::text() { return _text; }
string Card::imagePath() { return _imagePath; }

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
                _triggerMap[pair.key()] = std::make_pair(
                    v["check"],
                    v["effect"]
                );
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
std::pair<string, string> ScriptCard::getTriggerWhen(string triggerName) { return _triggerMap[triggerName]; }

string ScriptCard::enterFuncName() { return _enterFuncName; }
string ScriptCard::leaveFuncName() { return _leaveFuncName; }


string ScriptCard::costFuncName() { return _costFuncName; }
bool ScriptCard::isTrinket() { return _isTrinket; }
bool ScriptCard::goesToBottom() { return _goesToBottom; }

void ScriptCard::use(Player* player, Match* match) {
    if (_isTrinket) {
        //  TODO
    } else {
        match->execFunc(this->_useFuncName);
    }
}

CharacterCard::CharacterCard(string dir, json j) : 
    Card(dir, j),
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

MonsterCard::MonsterCard(string dir, json j) :
    Card(dir, j)
{
    //  TODO
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
    l_pushtablenumber(L, "id", (float)this->_id);
    l_pushtablenumber(L, "ownerID", (float)_owner->id());
    l_pushtableboolean(L, "tapped", _tapped);
}

CardState CardWrapper::getState() {
    CardState result;
    result.cardName = _card->name();
    result.active = !_tapped;
    result.id = _id;
    return result;
}
