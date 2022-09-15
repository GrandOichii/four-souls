function IAmError_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    if roll.value == 1 or roll.value == 2 then
    	--  TODO replace with prompt to attack

        addAttackOpportunity(host, owner.id, true, true, {-1})
    end
    if roll.value == 3 or roll.value == 4 then
        lootCards(host, owner.id, 3)
    end
    if roll.value == 5 or roll.value == 6 then
        gainTreasure(host, owner.id, 1)
    end
    Common_BonusMonsterTail(host, me.id)
end