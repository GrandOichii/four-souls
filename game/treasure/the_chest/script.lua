function TheChest_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)

    removeFromEverywhere(host, me.id)
    addSoulCard(host, owner.id, me.id)
end