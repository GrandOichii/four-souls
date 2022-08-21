function DiceShard_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID)
end

function DiceShard_use(host)
    local target = popTarget(host)
    local value = math.random(1, 6)
    setRollValue(host, target.id, value)
    discardMe(host, this(host).id)

end