function BlackPills_use(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 1 or roll == 2 then
        local target = Common_ChoosePlayer(host, owner.id)
        dealDamage(host, PLAYER, owner.id, PLAYER, target, 1)
        return
    end
    if roll == 3 or roll == 4 then
        local target = Common_ChooseMonster(host, owner.id)
        dealDamage(host, PLAYER, owner.id, MONSTER, target.id, 1)
        return
    end
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
    discardMe(host, this(host).id)

end