--  TODO untested

function BookOfVirtues_trigger1_check(host, me)
    return Common_OwnerDealtCombatDamage(host, me.id)
end

function BookOfVirtues_trigger1_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function BookOfVirtues_trigger1(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host)
    if roll.value <= 4 then
        return
    end
    local choice1 = 'Monster'
    local choice2 = 'Player'
    local choice = requestSimpleChoice(host, owner.id, 'Deal damage to what?', {choice1, choice2})
    if choice == choice1 then
        local target = Common_ChooseMonster(host, owner.id)
        dealDamage(host, PLAYER, owner.id, MONSTER, target.id, 1)
        return
    end
    local target = Common_ChoosePlayer(host, owner.id)
    dealDamage(host, PLAYER, owner.id, PLAYER, target, 1)
end

function BookOfVirtues_trigger2_check(host, me)
    return Common_OwnerKilledMonster(host, me.id)
end

function BookOfVirtues_trigger2(host)
    local owner = getTopOwner(host)
    local targetID = Common_ChooseOpponent(host, owner.id)
    lootCards(host, owner.id, 1)
    lootCards(host, targetID, 1)
end
