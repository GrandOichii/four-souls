function FourCents_use(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 4)
    discardMe(host, this(host).id)

end