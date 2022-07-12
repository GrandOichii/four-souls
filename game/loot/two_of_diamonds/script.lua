function TwoOfDiamonds_cost(host, cardInfo)
    local ownerID = cardInfo["ownerID"]
    local players = getPlayers(host)
    local ids = {}
    for i, p in pairs(players) do
        ids[i] = p["id"]
    end
    local choiceId, payed = requestChoice(host, ownerID, "Choose a player", PLAYER, ids)
    if not payed then return false end
    pushTarget(host, choiceId, PLAYER)
    return true
end

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