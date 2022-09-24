function Athame_cost(host, cardInfo)
    return Common_Discard(host, cardInfo.ownerID, 1, true) and Common_TargetPlayer(host, cardInfo.ownerID)
end

function Athame_force(host)
    local target = popTarget(host)
    Common_Discard(host, target.id, 2, false)
end
