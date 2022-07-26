function Justice_cost(host, cardInfo)

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

function Justice_use(host)

    local target = popTarget(host)
    local targetID = target.id

    local player = Common_PlayerWithID(host, targetID)

    local owner = getTopOwner(host)
    local diff = player["coins"] - owner["coins"]
    if diff > 0 then
        addCoins(host, owner["id"], diff)
    end
    diff = Common_CardCount(player) - Common_CardCount(owner)
    if diff > 0 then
        lootCards(host, owner["id"], diff)
    end
end