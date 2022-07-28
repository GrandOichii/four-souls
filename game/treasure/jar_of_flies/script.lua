function JarOfFlies_trigger_check(host, cardInfo)
    return Common_LastDeath(host).type == MONSTER
end

function JarOfFlies_trigger(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function JarOfFlies_cost(host, cardInfo)
    local me = this(host)
    if me.counters == 0 then
        return false
    end
    local flag = Common_Tap(host)
    if not flag then
        return false
    end
    local choices = {}
    for i = 1, me.counters do
        choices[#choices+1] = tostring(i)
    end
    local choice = requestSimpleChoice(host, cardInfo.ownerID, 'Remove how many counters?', choices)
    local amount = tonumber(choice)
    CardData[me.id] = {
        removed = amount
    }
    Common_RemoveCounters(host, amount)
    if amount >= 3 then
        return Common_TargetMonster(host, cardInfo.ownerID)
    end
    return true
end

function JarOfFlies_tap(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local amount = CardData[me.id].removed
    if amount >= 3 then
        local target = popTarget(host).id
        dealDamage(host, PLAYER, owner.id, MONSTER, target, 1)
        return
    end
    lootCards(host, owner.id, 1)
end
