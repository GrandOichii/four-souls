--  TODO untested

function TheD6_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo["ownerId"]) and Common_Tap(host)
end

function TheD6_tap(host)
    local target = popTarget(host)
    setRollValue(host, target["id"], math.random(1, 6))
end