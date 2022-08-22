function APenny_use(host)
    discardMe(host, this(host).id)
    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 1)
    

end