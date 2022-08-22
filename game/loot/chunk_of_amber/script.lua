function ChunkOfAmber_use(host)
    discardMe(host, this(host).id)
    local roll = popRollStack(host).value
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 2 * roll)
    

end