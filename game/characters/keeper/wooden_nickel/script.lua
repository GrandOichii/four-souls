function WoodenNickel_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function WoodenNickel_tap(host)
    local target = popTarget(host)
    local roll = popRollStack(host).value
    addCoins(host, target.id, roll)
end