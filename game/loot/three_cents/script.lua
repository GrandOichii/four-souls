function ThreeCents_use(host)
    discardMe(host, this(host).id)

    local owner = getTopOwner(host)
    addCoins(host, owner.id, 3)
end