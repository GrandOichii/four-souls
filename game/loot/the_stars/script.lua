function TheStars_use(host)
    local owner = getTopOwner(host)
    plusOneTreasure(host, owner["id"])
end