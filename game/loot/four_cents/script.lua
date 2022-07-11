function FourCents_use(host)

    local owner = getOwner(host)
    addCoins(host, owner["id"], 4)

end