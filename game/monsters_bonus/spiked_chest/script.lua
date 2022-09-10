function SpikedChest_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    if roll.value == 3 or roll.value == 4 then
        lootCards(host, owner.id, 1)
    end
    if roll.value == 5 or roll.value == 6 then
        gainTreasure(host, owner.id, 1)
    end
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
    Common_BonusMonsterTail(host, me.id)
end