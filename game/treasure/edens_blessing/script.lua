function EdensBlessing_trigger_check(host, me)
    local owner = getOwner(host, me["id"])
    local currentPlayer = getCurrentPlayer(host)
    if owner["id"] ~= currentPlayer["id"] then
        return false
    end
    local coins = owner["coins"]
    return coins == 0
end

function EdensBlessing_trigger(host)
    local owner = getOwner(host)
    addCoins(host, owner["id"], 6)
end