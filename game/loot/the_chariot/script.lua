function TheChariot_cost(host, cardInfo)

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

function TheChariot_use(host)

    local target = popTarget(host)
    local targetID = target["id"]
    local players = getPlayers(host)

    for _, p in pairs(players) do
        if p["id"] == targetID then
            tempIncMaxLife(host, targetID, 1)
            tempIncAttack(host, targetID, 1)
            return
        end
    end

end