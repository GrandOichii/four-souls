function APenny_use(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 1)
    discardMe(host, this(host).id)

end