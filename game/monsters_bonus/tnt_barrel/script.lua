function TNTBarrel_cost(host, info)
    return Common_TargetMonsterOrPlayer(host, info.ownerID)
end

function TNTBarrel_effect(host)
    local me = this(host)
    local roll = popRollStack(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    local amount = 4
    if roll.value < 5 then
        amount = 2
        dealDamage(host, NO_SOURCE, NO_SOURCE_ID, target.type, target.id, amount)
    end
    dealDamage(host, NO_SOURCE, NO_SOURCE_ID, target.type, target.id, amount)
    Common_BonusMonsterTail(host, me.id)
end
