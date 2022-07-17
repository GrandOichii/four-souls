--  TODO untested

function GuppysPaw_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo) and Common_Tap(host)
end

function GuppysPaw_tap(host)
    local owner = getTopOwner(host)
    dealDamage(host, PLAYER, owner["id"], PLAYER, owner["id"], 1)
    local target = popTarget(host)
    addBlueHealth(host, target["id"], 2)
end