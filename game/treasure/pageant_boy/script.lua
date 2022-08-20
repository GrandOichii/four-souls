function PageantBoy_cost(host, cardInfo)
    local me = this(host)
    destroyCard(host, me.id)
    return true
end

function PageantBoy_tap(host)
    local roll = popRollStack(host).value
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 4 * roll)
end