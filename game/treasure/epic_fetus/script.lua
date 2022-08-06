function EpicFetus_cost(host, cardInfo)
    return Common_Tap(host) and Common_TargetPlayer(host, cardInfo) and Common_TargetMonster(host, cardInfo.ownerID)
end

function EpicFetus_tap(host)
    local owner = getTopOwner(host)
    local target1 = popTarget(host)
    dealDamage(host, PLAYER, owner.id, MONSTER, target1.id, 1)
    local target2 = popTarget(host)
    dealDamage(host, PLAYER, owner.id, PLAYER, target2.id, 1)
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
end