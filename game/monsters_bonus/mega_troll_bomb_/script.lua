function MegaTrollBomb_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    for _, player in ipairs(getPlayers(host)) do
        dealDamage(host, NO_SOURCE, NO_SOURCE_ID, PLAYER, player.id, 2)
    end
    Common_BonusMonsterTail(host, me.id)
end