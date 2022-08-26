#include "types.hpp"

#include "match.hpp"
#include "cards.hpp"
#include "player.hpp"

Effect::Effect() {

}

Effect::Effect(json j) {
    funcName = j["effect"];
    if (j.contains("text"))
        text = j["text"];
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

EffectState Effect::getState() const {
    EffectState result;
    result.text = text;
    return result;
}


Trigger::Trigger() :
    Effect() {}

Trigger::Trigger(json j) :
    Effect(j)
{
    checkFuncName = j["check"];
}

StackEffect* Effect::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) {
    if (hasRequirement(TAP_REQUIREMENT) && !cardW->isActive()) return nullptr;
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
    if (hasRequirement(TAP_REQUIREMENT)) cardW->tap();
    if (hasRequirement(ROLL_REQUIREMENT)) {
        match->pushToStack(new StackEffect(
            "_activateRoll",
            owner, 
            nullptr,
            ROLL_TYPE
        ));
        match->_stack.erase(std::find(match->_stack.begin(), match->_stack.end(), p));
        RollEvent re(
            owner,
            false,
            p
        );
        match->log(owner->name() + " rolls a " + std::to_string(re.value));
        match->_rollStack.push_back(re);
        match->applyTriggers(ROLL_TYPE);
        return p;
    }
    if (!usesStack) {
        match->execFunc(funcName);
        match->_stack.erase(std::find(match->_stack.begin(), match->_stack.end(), p));
        delete p;
        return nullptr;
    }
    match->applyTriggers(type);
    return p;
}

StackEffect* Trigger::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) {
    return Effect::pushMe(match, cardW, owner, type);
}