function TrollBombs_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 2)
    Common_BonusMonsterTail(host, me.id)
end