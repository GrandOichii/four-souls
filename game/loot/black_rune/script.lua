function BlackRune_use(host)
    discardMe(host, this(host).id)

    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 1 or roll == 2 then
        local monsters = getActiveMonsters(host)
        for _, monster in ipairs(monsters) do
            dealDamage(host, PLAYER, owner.id, MONSTER, monster.id, 1)
        end
        return
    end
    if roll == 3 or roll == 4 then
        local choice, chosen = Common_ChooseNonEternalCard(host, owner.id)
        if not chosen then
            return
        end
        Common_RerollItem(host, choice) 
        return
    end
    for _, card in ipairs(owner.hand) do
        discardLoot(host, owner.id, card.id)
    end
    lootCards(host, owner.id, 3)

end