--  TODO untested

function ChunkOfAmber_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function ChunkOfAmber_use(host)
    local roll = getLastRoll(host).value
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 2 * roll)
end