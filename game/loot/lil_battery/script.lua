function LilBattery_cost(host, cardInfo)
    return Common_TargetTappedCard(host, cardInfo.ownerID)
end

function LilBattery_use(host)
    discardMe(host, this(host).id)
    local target = popTarget(host)
    rechargeCard(host, target.id)
    

end