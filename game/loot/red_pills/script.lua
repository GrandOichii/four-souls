function RedPills_use(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 1 or roll == 2 then
        Common_TempIncAttack(host, me.id, owner.id, 1)
        return
    end
    if roll == 3 or roll == 4 then
        --  TODO untested
        Common_TempIncMaxLife(host, me.id, owner.id, 1)
        return
    end
    if roll == 5 or roll == 6 then
        dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
        return
    end
    discardMe(host, this(host).id)

end