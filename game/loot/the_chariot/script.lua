function TheChariot_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function TheChariot_use(host)
    local me = this(host)
    local target = popTarget(host)
    Common_TempIncMaxLife(host, me.id, target.id, 1)
    Common_TempIncAttack(host, me.id, target.id, 1)
    discardMe(host, this(host).id)
end