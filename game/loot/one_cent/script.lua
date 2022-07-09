function APenny_use(host)

    local owner = getOwner(host)
    addCoins(host, owner["id"], 1)

end