function Death_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function Death_use(host)
    local target = popTarget(host)
    killEntity(host, PLAYER, target.id)
    discardMe(host, this(host).id)

end