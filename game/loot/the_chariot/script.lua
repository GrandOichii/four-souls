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
    local me = this(host)
    local target = popTarget(host)
    local targetID = target["id"]

    --  TODO untested
    Common_TempIncMaxLife(host, me.id, target.id, 1)
    Common_TempIncAttack(host, me.id, target.id, 1)

end