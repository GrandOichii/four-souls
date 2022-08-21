function TwoCents_use(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 2)
    discardMe(host, this(host).id)

end