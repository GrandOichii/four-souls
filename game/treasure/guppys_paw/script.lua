--  TODO untested

function GuppysPaw_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID) and Common_Tap(host)
end

function GuppysPaw_tap(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
    addBlueHealth(host, target.id, 2)
end