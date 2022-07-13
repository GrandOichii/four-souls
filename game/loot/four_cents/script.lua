function FourCents_use(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 4)

end