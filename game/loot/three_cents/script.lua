function ThreeCents_use(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 3)

end