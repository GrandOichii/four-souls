function StarterDeck_trigger_check(host, me)
    local owner = getCardOwner(host, me["id"])
    local currentPlayer = getCurrentPlayer(host)
    if owner["id"] ~= currentPlayer["id"] then
        return false
    end
    local cards = owner["hand"]
    local count = 0
    for _ in pairs(cards) do count = count + 1 end
    return count >= 8
end

function StarterDeck_trigger(host)
    local owner = getOwner(host)
    lootCards(host, owner["id"], 2)
end