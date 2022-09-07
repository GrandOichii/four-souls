function PandorasBox_tap(host)
    local me = this(host)
    local roll = popRollStack(host).value
    local owner = getTopOwner(host)
    local done = Common_SacrificeMe(host, me.id)
    if not done then
        return
    end
    if roll == 1 then
        addCoins(host, owner.id, 1)
        return
    end
    if roll == 2 then
        addCoins(host, owner.id, 6)
        return
    end
    if roll == 3 then
        local monster = Common_ChooseMonster(host, owner.id)
        killEntity(host, MONSTER, monster.id)
        return
    end
    if roll == 4 then
        lootCards(host, owner.id, 3)
        return
    end
    if roll == 5 then
        addCoins(host, owner.id, 9)
        return
    end
    if roll == 6 then
        removeFromEverywhere(host, me.id)
        addSoulCard(host, owner.id, me.id)
        return
    end
end