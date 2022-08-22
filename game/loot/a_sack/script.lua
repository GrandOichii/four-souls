function ASack_use(host)
    local me = this(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 3)
    discardMe(host, me.id)

end