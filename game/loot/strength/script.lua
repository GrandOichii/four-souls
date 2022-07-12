function Strength_cost(host, cardInfo)

    return Common_TargetPlayer(host, cardInfo)

end

function Strength_use(host)

    local target = popTarget(host)
    local targetID = target["id"]

    tempIncAttack(host, targetID, 1)
    incAttackCount(host, targetID)
end