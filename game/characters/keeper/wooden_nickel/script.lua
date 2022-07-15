function WoodenNickel_cost(host, cardInfo)
    return Common_Tap(host) and Common_TargetPlayer(host, cardInfo) and Common_Roll(host, cardInfo["ownerID"])
end

function WoodenNickel_tap(host)
    local target = popTarget(host)
    local roll = getLastRoll(host)["value"]
    addCoins(host, target["id"], roll)
end