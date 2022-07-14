function TwoOfDiamonds_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo)
end

function TwoOfDiamonds_use(host)
    local targetID = popTarget(host)["id"]
    print(targetID)
    local player = Common_PlayerWithID(host, targetID)
    addCoins(host, targetID, player["coins"])
end