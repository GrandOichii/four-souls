function AceOfDiamonds_use(host)
    local owner = getTopOwner(host)
    local amount = #getActiveMonsters(host) + #Common_PlayerWithID(host, owner.id).hand
    addCoins(host, owner.id, amount)
    discardMe(host, this(host).id)

end