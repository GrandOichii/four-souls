function Death_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function Death_use(host)
    discardMe(host, this(host).id)

    local target = popTarget(host)
    killEntity(host, PLAYER, target.id)

end