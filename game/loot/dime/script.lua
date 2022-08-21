function Dime_use(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 10)
    discardMe(host, this(host).id)

end