function XLFloor_effect(host)
    local me = this(host)
    local player = getCurrentPlayer(host)
    expandActiveMonsters(host, 1)
    Common_IncAttackCount(host, player.id, false)
    Common_BonusMonsterTail(host, me.id)
end