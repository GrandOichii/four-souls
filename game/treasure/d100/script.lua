function D100_tap(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 1 then
        lootCards(host, owner.id, 1)
    end
    if roll == 2 then
        lootCards(host, owner.id, 2)
    end
    if roll == 3 then
        addCoins(host, owner.id, 3)
    end
    if roll == 4 then
        addCoins(host, owner.id, 4)
    end
    if roll == 5 then
        Common_TempIncMaxLife(host, me.id, owner.id, 1)
    end
    if roll == 6 then
        Common_TempIncAttack(host, me.id, owner.id, 1)
    end
end