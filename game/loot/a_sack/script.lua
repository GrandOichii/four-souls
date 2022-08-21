function ASack_use(host)
    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 3)
    discardMe(host, this(host).id)

end