function ThePolaroid_trigger_check(host, me)
    local owner = getOwner(host, me["id"])
    local currentPlayer = getCurrentPlayer(host)
    if owner["id"] ~= currentPlayer["id"] then
        return false
    end
    local cards = owner["hand"]
    local count = 0
    for _ in pairs(cards) do count = count + 1 end
    return count == 0
end

function ThePolaroid_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 2)
end