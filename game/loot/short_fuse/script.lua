function ShortFuse_cost(host, info)
    return Common_TargetMonsterOrPlayer(host, info.ownerID)
end

function ShortFuse_effect(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    dealDamage(host, PLAYER, owner.id, target.type, target.id, 2)
end
