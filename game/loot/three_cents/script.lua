function ThreeCents_use(host)

    local owner = getOwner(host)
    addCoins(host, owner["id"], 3)

end