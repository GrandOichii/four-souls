function Jera_use(host)
    discardMe(host, this(host).id)

    local owner = getTopOwner(host)
    local count = Common_CardCount(owner)
    lootCards(host, owner["id"], count)

end