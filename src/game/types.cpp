#include "types.hpp"

#include "match.hpp"
#include "cards.hpp"
#include "player.hpp"

Effect::Effect() {

}

Effect::Effect(json j) {
    funcName = j["effect"];
    if (j.contains("cost"))
        costFuncName = j["cost"];
    if (j.contains("usesStack"))
        usesStack = j["usesStack"];
    if (j.contains("requires")) {
        for (const auto& [key, value] : j["requires"].items())
            requirements.push_back(value);
    }
}

bool Effect::hasRequirement(string req) {
    for (const auto& r : requirements)
        if (req == r) return true;
    return false;
}

Trigger::Trigger() :
    Effect() {}

Trigger::Trigger(json j) :
    Effect(j)
{
    checkFuncName = j["check"];
}

StackEffect* Effect::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) {
    if (hasRequirement(TAP_REQUIREMENT)){
        if (!cardW->isActive())return nullptr;
        cardW->tap();
    } 
    if (!funcName.size()) return nullptr;
    auto p = new StackEffect(
        funcName, 
        owner, 
        cardW,
        type    
    );
    match->pushToStack(p);
    if (costFuncName.size()) {
        bool payed = match->requestPayCost(costFuncName, owner ? owner : match->_activePlayer);
        if (!payed) {
            match->_stack.erase(std::find(match->_stack.begin(), match->_stack.end(), p));
            delete p;
            return nullptr;
        }
    }
    if (!usesStack) {
        match->execFunc(funcName);
        match->_stack.erase(std::find(match->_stack.begin(), match->_stack.end(), p));
        delete p;
        return nullptr;
    }
    return p;
}

StackEffect* Trigger::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) {
    return Effect::pushMe(match, cardW, owner, type);
}