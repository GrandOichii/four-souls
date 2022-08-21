function TheStars_use(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
    discardMe(host, this(host).id)

end