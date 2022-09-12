function TheCompass_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TheCompass_trigger(host)
    local owner = getTopOwner(host)
    Common_OrderTopCards(host, owner.id, LOOT_DECK, 4)
end
