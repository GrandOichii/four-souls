function TheLovers_cost(host, cardInfo)

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

function TheLovers_use(host)
    local target = popTarget(host)
    local targetID = target["id"]
    tempIncMaxLife(host, targetID, 2)
end