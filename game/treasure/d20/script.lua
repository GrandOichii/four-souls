function D20_cost(host, cardInfo)
    return Common_TargetNonEternalCard(host, cardInfo.ownerID)
end

function D20_tap(host)
    local target = popTarget(host)
    Common_RerollItem(host, target.id)
end