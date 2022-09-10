function AngelRoom_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    if roll.value == 1 then
        gainTreasure(host, owner.id, 1)
    end
    if roll.value == 2 or roll.value == 3 then
        gainTreasure(host, owner.id, 2)
    end
    if roll.value > 3 then
        lootCards(host, owner.id, 2)
    end
	Common_BonusMonsterTail(host, me.id)
end
