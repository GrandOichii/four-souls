function TrapDoor_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    if roll.value < 4 then
        Common_OrderTopCards(host, owner.id, MONSTER_DECK)
        Common_IncAttackCount(host, owner.id, false)
    else
        dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
    end
    Common_BonusMonsterTail(host, me.id)
end
