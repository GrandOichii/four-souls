function SuicideKing_trigger_check(host, cardInfo)
    return Common_OwnerDied(host, cardInfo.ownerID)
end

function SuicideKing_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 3)
end