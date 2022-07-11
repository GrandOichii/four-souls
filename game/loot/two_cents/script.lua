function TwoCents_use(host)

    local owner = getOwner(host)
    addCoins(host, owner["id"], 2)

end