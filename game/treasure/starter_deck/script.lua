function StarterDeck_trigger_check(host, me)
    local owner = getOwner(host, me.id)
    local currentPlayer = getCurrentPlayer(host)
    if owner.id ~= currentPlayer.id then
        return false
    end
    local count = Common_CardCount(owner)
    return count >= 8
end

function StarterDeck_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
end