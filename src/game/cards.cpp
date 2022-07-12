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

ScriptCard::ScriptCard(string dir, json j) : 
    Card(dir, j)
{ 
    this->_script = fs::readFile(fs::join(dir, j["script"]).c_str());
}

string ScriptCard::script() { return _script; }

TrinketCard::TrinketCard(string dir, json j, bool isEternal) :
    ScriptCard(dir, j),
    _isEternal(isEternal)
{
    std::cout << _name << std::endl;
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
}

void TrinketCard::print(string prefix) {
    ScriptCard::print(prefix);
    std::cout << prefix << "Is eternal: " << _isEternal << std::endl;
    std::cout << prefix << "Enter script:" << std::endl << prefix << "\t" << _enterFuncName << std::endl;
    std::cout << prefix << "Leave script:" << std::endl << prefix << "\t" << _leaveFuncName << std::endl;
    std::cout << prefix << "trigger map:" << std::endl;
    for (const auto& [key, value] : _triggerMap) {
        std::cout << prefix << "\t" << key << " : " << value.second << std::endl;
    }
    std::cout << prefix << "Activations:" << std::endl;
    for (auto& ability : _abilities) {
        ability.print();
    }
}

std::vector<ActivatedAbility> TrinketCard::abilities() { return _abilities; }
bool TrinketCard::hasTrigger(string triggerName) { return _triggerMap.count(triggerName); }
std::pair<string, string> TrinketCard::getTriggerWhen(string triggerName) { return _triggerMap[triggerName]; }

string TrinketCard::enterFuncName() { return _enterFuncName; }
string TrinketCard::leaveFuncName() { return _leaveFuncName; }


LootCard::LootCard(string dir, json j, bool isTrinket) : 
    ScriptCard(dir, j),
    _isTrinket(isTrinket) 
{
    if (!_isTrinket) {
        this->_useFuncName = j["use"];
        if (j.contains("goes_to_bottom"))
            this->_goesToBottom = j["goes_to_bottom"];
        if (j.contains("cost"))
            this->_costFuncName = j["cost"];
        return;
    }
    this->_trinket = new TrinketCard(dir, j, false);
}

LootCard::~LootCard() {
    delete _trinket;
}

void LootCard::print(string prefix) {
    ScriptCard::print(prefix);
    if (_isTrinket) {
        std::cout << "Trinket:" << std::endl;
        this->_trinket->print("\t");
    } else {
        std::cout << "Use: " << std::endl << "\t" << _useFuncName << std::endl;;
    }
}

string LootCard::costFuncName() { return _costFuncName; }
bool LootCard::isTrinket() { return _isTrinket; }
bool LootCard::goesToBottom() { return _goesToBottom; }

void LootCard::use(Player* player, Match* match) {
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
    this->_startingItem = new TrinketCard(itemDir, jj, true);
}

int CharacterCard::attack() { return _attack; }
int CharacterCard::health() { return _health; }
TrinketCard* CharacterCard::startingItem() { return _startingItem; }

MonsterCard::MonsterCard(string dir, json j) :
    Card(dir, j)
{
    //  TODO
}

CardWrapper::CardWrapper(Card* card, int id) : 
    _card(card),
    _id(id) {}

Card* CardWrapper::card() { return _card; }
int CardWrapper::id() { return _id; }
void CardWrapper::recharge() { _tapped = false; }
void CardWrapper::tap() { _tapped = true; }
bool CardWrapper::isActive() { return !_tapped; }

void CardWrapper::pushTable(lua_State* L) {
    lua_newtable(L);
    l_pushtablenumber(L, "id", (float)this->_id);
    l_pushtableboolean(L, "tapped", _tapped);
}