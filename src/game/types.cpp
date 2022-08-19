#include "types.hpp"

#include "match.hpp"
#include "cards.hpp"
#include "player.hpp"

StackEffect* Effect::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) {
    match->_stack.pop_back();
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

StackEffect* ActivatedAbility::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) {
    return Effect::pushMe(match, cardW, owner, type);
}

StackEffect* Trigger::pushMe(Match* match, CardWrapper* cardW, Player* owner, string type) {
    return Effect::pushMe(match, cardW, owner, type);
}