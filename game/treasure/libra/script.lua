function Libra_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID)
end

function Libra_tap(host)
    local target = popTarget(host)
    setRollValue(host, target.id, 3)
end