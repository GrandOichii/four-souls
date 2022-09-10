function ShopUpgrade_effect(host)
    local me = this(host)
    local player = getCurrentPlayer(host)
    Common_IncAttackCount(host, player.id, false)
    expandShopSize(host, 2)
    Common_BonusMonsterTail(host, me.id)
end
