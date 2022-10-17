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
    if (j.contains("check"))
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
        auto roller = owner ? owner : match->getCurrentPlayer();
        match->pushToStack(new StackEffect(
            "_activateRoll",
            roller, 
            nullptr,
            ROLL_TYPE
        ));
        match->_stack.erase(std::find(match->_stack.begin(), match->_stack.end(), p));
        RollEvent re(
            roller,
            false,
            p
        );
        match->log(roller->name() + " rolls a " + std::to_string(re.value));
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

void Effect::pushTable(lua_State* L) {
    lua_newtable(L);
    if (!funcName.size()) return;
    l_pushtablestring(L, "cost", costFuncName);
    l_pushtablestring(L, "effect", funcName);
    l_pushtablestring(L, "text", funcName);
    l_pushtableboolean(L, "usesStack", usesStack);
    auto size = requirements.size();
    lua_createtable(L, size, 0);
    for (int i = 0; i < size; i++) {
        lua_pushnumber(L, i+1);
        lua_pushstring(L, requirements[i].c_str());
        lua_settable(L, -3);
    }
}

StackEffect* Trigger::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) {
    return Effect::pushMe(match, cardW, owner, type);
}

void Trigger::pushTable(lua_State* L) {
    Trigger::pushTable(L);
    l_pushtablestring(L, "check", checkFuncName);
}