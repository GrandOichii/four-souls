function PageantBoy_cost(host, cardInfo)
    local me = this(host)
    destroyCard(host, me.id)
    return Common_Roll(host, cardInfo.ownerID)
end

function PageantBoy_tap(host)
    local roll = getLastRoll(host).value
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 4 * roll)
end