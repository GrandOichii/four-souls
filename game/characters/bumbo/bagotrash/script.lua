function Bagotrash_cost1(host, info)
    return Common_Pay(host, info.ownerID, 3)
end

function Bagotrash_effect1(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end

function Bagotrash_cost2(host, info)
    return Common_Pay(host, info.ownerID, 4)
end

function Bagotrash_effect2(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end

function Bagotrash_cost3(host, info)
    return Common_Pay(host, info.ownerID, 6) and Common_TargetMonsterOrPlayer(host, info.ownerID)
end

function Bagotrash_effect3(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    dealDamage(host, PLAYER, owner.id, target.type, target.id, 1)
end