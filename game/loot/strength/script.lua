function Strength_cost(host, cardInfo)

    return Common_TargetPlayer(host, cardInfo.ownerID)

end

function Strength_use(host)
    local me = this(host)
    local target = popTarget(host)

    Common_TempIncAttack(host, me.id, target.id, 1)
    incAttackCount(host, target.id)
    discardMe(host, this(host).id)

end