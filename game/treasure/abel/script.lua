function Abel_trigger_check(host, me)
    return Common_OwnerDealtCombatDamage(host, me.id, MONSTER)
end

function Abel_trigger_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function Abel_trigger(host)
    local dEvent = getDamageEvent(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Deal damage to what?', {'Monster', 'Player'})
    if choice == 'Monster' then
        local monsters = getActiveMonsters(host)
        local ids = {}
        for _, monster in ipairs(monsters) do
            if monster.id ~= dEvent.targetID or dEvent.targetType ~= MONSTER then
                ids[#ids+1] = monster.id
            end
        end
        local choiceId, payed = requestChoice(host, owner.id, "Choose a monster", MONSTER, ids)
        if not payed then return false end
        dealDamage(host, PLAYER, owner.id, MONSTER, choiceId, 1)
        return
    end
    if choice == 'Player' then
        local choice = Common_ChoosePlayer(host, owner.id)
        dealDamage(host, PLAYER, owner.id, PLAYER, choice, 1)
        return
    end
end