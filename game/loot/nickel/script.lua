function ANickel_use(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 5)
    discardMe(host, this(host).id)

end