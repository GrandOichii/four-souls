function TheBattery_cost(host, cardInfo)
    return Common_TargetTappedCard(host, cardInfo["ownerID"])
end

function TheBattery_tap(host)
    local target = popTarget(host)
    rechargeCard(host, target["id"])
end