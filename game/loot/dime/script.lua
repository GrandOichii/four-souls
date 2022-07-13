function Dime_use(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 10)

end