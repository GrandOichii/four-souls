function TheBlueMap_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TheBlueMap_trigger(host)
    local owner = getTopOwner(host)
    Common_OrderTopCards(host, owner.id, TREASURE_DECK, 4)
end
