function TheD6_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID) and Common_Tap(host)
end

function TheD6_tap(host)
    local target = popTarget(host)
    local value = math.random(1, 6)
    setRollValue(host, target.id, value)
end