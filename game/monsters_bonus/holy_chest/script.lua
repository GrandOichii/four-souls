function HolyChest_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    if roll.value == 1 or roll.value == 2 then
        addBlueHealth(host, owner.id, 2)
        Common_IncAttackCount(host, owner.id, false)
    end
    if roll.value > 2 and roll.value < 6 then
        addCoins(host, owner.id, 7)
    end
    if roll.value == 6 then
        removeFromEverywhere(host, me.id)
        addSoulCard(host, owner.id, me.id)
        return
    end
    Common_BonusMonsterTail(host, me.id)
end
