function DarkChest_effect(host)
    local me = this()
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    if roll.value == 1 or roll.value == 2 then
        lootCards(host, owner.id, 1)
    end
    if roll.value == 3 or roll.value == 4 then
        addCoins(host, owner.id, 3)
    end
    if roll.value == 5 or roll.value == 6 then
        dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 2)
    end
	Common_BonusMonsterTail(host, me.id)
end