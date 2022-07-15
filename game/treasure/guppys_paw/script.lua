--  TODO untested

function GuppysPaw_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo) and Common_PayOneLife(host, cardInfo["ownerID"]) and Common_Tap(host)
end

function GuppysPaw_tap(host)
    local target = popTarget(host)
    addBlueHealth(host, target["id"], 2)
end