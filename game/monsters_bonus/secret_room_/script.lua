function SecretRoom_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    if roll.value == 1 then
        dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 3)
    end
    if roll.value == 2 or roll.value == 3 then
        Common_Discard(host, owner.id, 2)
    end
    if roll.value == 4 or roll.value == 5 then
        addCoins(host, owner.id, 7)
    end
    if roll.value == 6 then
        gainTreasure(host, owner.id, 1)
    end
    Common_BonusMonsterTail(host, me.id)
end
