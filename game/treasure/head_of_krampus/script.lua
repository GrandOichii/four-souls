function HeadOfKrampus_tap(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll < 4 then
        local players = getPlayers(host)
        for _, player in ipairs(players) do
            dealDamage(host, PLAYER, owner.id, PLAYER, player.id, 1)
        end
        return
    end
    local monsters = getActiveMonsters(host)
    for _, monster in ipairs(monsters) do
        dealDamage(host, PLAYER, owner.id, MONSTER, monster.id, 1)
    end
end