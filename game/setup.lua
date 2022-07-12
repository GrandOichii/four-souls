function Common_TargetPlayer(host, cardInfo)
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

function Common_PlayerWithID(host, id)
    local players = getPlayers(host)
    for _, p in pairs(players) do
        if p["id"] == id then
            return p
        end
    end
end

function Common_CardCount(host, player)
    local count = 0
    for _ in pairs(player["hand"]) do count = count + 1 end
    return count
end