function TheWiz_enter(host, me, owner)
    CardData[me.id] = {
        irID = Common_IncMonsterRolls(owner.id, 1)
    }
end

function TheWiz_leave(host, me, owner)
    Common_DecMonsterRolls(host, CardData[me.id].irID, owner.id)
end

function TheWiz_trigger_check(host, me)
    local owner = getOwner(host, me.id)
    local damageEvent = getTopDamageEvent(host)
    if not damageEvent.isCombatDamage then
        return false
    end
    if damageEvent.sourceType ~= PLAYER then
        return false
    end
    if damageEvent.sourceID ~= owner.id then
        return false
    end
    local choice = requestSimpleChoice(host, owner.id, 'Deal damage to what?', {'Monster', 'Player'})
    if choice == 'Monster' then
        local monsters = getActiveMonsters(host)
        local ids = {}
        print(damageEvent.targetID)
        print(damageEvent.targetType)
        for _, monster in ipairs(monsters) do
            print('\t', monster.id)
            if monster.id ~= damageEvent.targetID or damageEvent.targetType ~= MONSTER then
                ids[#ids+1] = monster.id
            end
        end
        print(#ids)
        local choiceId, payed = requestChoice(host, owner.id, "Choose a monster", MONSTER, ids)
        if not payed then return false end
        pushTarget(host, choiceId, MONSTER)
        return true
    end
    if choice == 'Player' then
        --  TODO? another player
        return Common_TargetPlayer(host, me)
    end
end

function TheWiz_trigger(host)
    local owner = getTopOwner(host)
    local _ = getDamageEvent(host)
    local target = popTarget(host)
    dealDamage(host, PLAYER, owner.id, target.type, target.id, 1)
end
