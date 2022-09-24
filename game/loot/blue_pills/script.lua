function BluePills_use(host, me)
    discardMe(host, this(host).id)

    local roll = popRollStack(host).value
    local owner = getTopOwner(host)
    if roll == 1 or roll == 2 then
        lootCards(host, owner.id, 1)
        return
    end
    if roll == 3 or roll == 4 then
        lootCards(host, owner.id, 3)
        return
    end
    Common_Discard(host, owner.id, 1, false)

end