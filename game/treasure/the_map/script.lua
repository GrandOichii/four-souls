function TheMap_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TheMap_trigger(host)
    local owner = getTopOwner(host)
    Common_OrderTopCards(host, owner.id, MONSTER_DECK, 4)
end
