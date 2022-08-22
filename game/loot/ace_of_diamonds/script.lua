function AceOfDiamonds_use(host)
    discardMe(host, this(host).id)
    local owner = getTopOwner(host)
    local amount = #getActiveMonsters(host) + #Common_PlayerWithID(host, owner.id).hand
    addCoins(host, owner.id, amount)

end