function TheTower_use(host)
    discardMe(host, this(host).id)

    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 1 or roll == 2 then
        local players = getPlayers(host)
        for _, player in ipairs(players) do
            dealDamage(host, NO_SOURCE, NO_SOURCE_ID, PLAYER, player.id, 1)
        end
        return
    end
    if roll == 3 or roll == 4 then
        local monsters = getActiveMonsters(host)
        for _, monster in pairs(monsters) do
            dealDamage(host, NO_SOURCE, NO_SOURCE_ID, MONSTER, monster.id, 1)
        end
        return
    end
    if roll == 5 or roll == 6 then
        local players = getPlayers(host)
        for _, player in ipairs(players) do
            dealDamage(host, NO_SOURCE, NO_SOURCE_ID, PLAYER, player.id, 2)
        end
        return
    end

end