function BatteryBum_cost(host, cardInfo)
    return Common_Pay(host, cardInfo.ownerID, 4) and Common_TargetTappedCard(host, cardInfo.ownerID)
end

function BatteryBum_pay(host)
    local target = popTarget(host)
    rechargeCard(host, target.id)
end
