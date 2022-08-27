function PayToPlay_cost(host, info)
    return Common_Pay(host, info.ownerID, 10) and Common_TargetNonEternalCard(host, info.ownerID)
end

function PayToPlay_effect(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    removeFromEverywhere(host, target.id)
    moveToBoard(host, owner.id, target.id)
end
