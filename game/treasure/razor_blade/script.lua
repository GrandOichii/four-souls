--  TODO untested

function RazorBlade_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID) and Common_Tap(host)
end

function RazorBlade_tap(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    dealDamage(host, PLAYER, owner["id"], PLAYER, target["id"], 1)
end