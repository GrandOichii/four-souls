function TheBone_tap(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function TheBone_cost1(host, info)
    return Common_RemoveCounter(host) and Common_TargetRoll(host, info.ownerID)
 end

function TheBone_effect1(host)
    local target = popTarget(host)
    local roll = Common_RollWithID(host, target.id)
    setRollValue(host, target.id, roll.value + 1)
end

function TheBone_cost2(host, info)
    return Common_RemoveCounters(host, 2) and Common_TargetMonsterOrPlayer(host, info.ownerID)
end

function TheBone_effect2(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    dealDamage(host, PLAYER, owner.id, target.type, target.id, 1)
end

function TheBone_cost3(host, info)
    return Common_RemoveCounters(host, 5)
end

function TheBone_effect3(host)
    local me = this(host)
    local owner = getTopOwner(host)
    removeFromEverywhere(host, me.id)
    addSoulCard(host, owner.id, me.id)
end
