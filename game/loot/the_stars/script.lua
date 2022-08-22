function TheStars_use(host)
    discardMe(host, this(host).id)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
    

end