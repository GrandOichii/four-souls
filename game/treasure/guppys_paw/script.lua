--  TODO untested

function GuppysPaw_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo) and Common_Tap(host)
end

function GuppysPaw_tap(host)
    local owner = getTopOwner(host)
    dealDamage(host, owner["id"], 1)
    print("SMOKE")
    local target = popTarget(host)
    print("MAMA")
    addBlueHealth(host, target["id"], 2)
end