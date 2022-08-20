function TheWiz_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
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
    CardData[me.id].lde = damageEvent
    return true
end

function TheWiz_trigger_cost(host, cardInfo)
    local me = this(host)
    local damageEvent = CardData[me.id].lde
    local choice = requestSimpleChoice(host, cardInfo.ownerID, 'Deal damage to what?', {'Monster', 'Player'})
    if choice == 'Monster' then
        local monsters = getActiveMonsters(host)
        local ids = {}
        for _, monster in ipairs(monsters) do
            if monster.id ~= damageEvent.targetID or damageEvent.targetType ~= MONSTER then
                ids[#ids+1] = monster.id
            end
        end
        local choiceId, payed = requestChoice(host, cardInfo.ownerID, "Choose a monster", MONSTER, ids)
        if not payed then return false end
        pushTarget(host, choiceId, MONSTER)
        return true
    end
    if choice == 'Player' then
        return Common_TargetPlayer(host, me.ownerID)
    end
end

function TheWiz_trigger(host)
    local owner = getTopOwner(host)
    local _ = getDamageEvent(host)
    local target = popTarget(host)
    dealDamage(host, PLAYER, owner.id, target.type, target.id, 1)
end
