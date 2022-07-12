function TwoOfDiamonds_cost(host, cardInfo)
    local ownerID = cardInfo["ownerID"]
    local players = getPlayers(host)
    local realI = 1
    local ids = {}
    for _, p in pairs(players) do
        ids[realI] = p["id"]
        realI = realI + 1
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a player", PLAYER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, PLAYER)
    -- pushTarget(host, ownerID, PLAYER)
    -- print("cost payed")
    return true
end

-- requestChoice


function TwoOfDiamonds_use(host)
    local targetID = popTarget(host)["id"]
    local players = getPlayers(host)
    for _, p in pairs(players) do
        -- print("\t", targetID, "\t", p["id"])
        if p["id"] == targetID then
            addCoins(host, targetID, p["coins"])
            return
        end
    end
end