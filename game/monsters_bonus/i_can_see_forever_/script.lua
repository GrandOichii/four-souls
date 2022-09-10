function ICanSeeForever_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_OrderTopCards(host, owner.id, LOOT_DECK)
    lootCards(host, owner.id, 1)
    Common_BonusMonsterTail(host, me.id)
end
