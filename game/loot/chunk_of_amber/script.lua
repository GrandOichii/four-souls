function ChunkOfAmber_use(host)
    local roll = popRollStack(host).value
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 2 * roll)
    discardMe(host, this(host).id)

end