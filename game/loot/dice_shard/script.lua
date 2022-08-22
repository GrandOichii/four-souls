function DiceShard_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID)
end

function DiceShard_use(host)
    discardMe(host, this(host).id)
    local target = popTarget(host)
    local value = math.random(1, 6)
    setRollValue(host, target.id, value)
    

end