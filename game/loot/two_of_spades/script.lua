--  TODO untested

function TwoOfSpades_cost(host, cardInfo)
    return Common_TargetOpponent(host, cardInfo.ownerID)
end

function TwoOfSpades_use(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, #player.hand)
end
