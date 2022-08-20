function MysterySack_tap(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 1 or roll == 2 then
        lootCards(host, owner.id, 1)
    end
    if roll == 3 or roll == 4 then
        addCoins(host, owner.id, 4)
    end
end
