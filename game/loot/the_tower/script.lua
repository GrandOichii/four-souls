--  TODO untested

function TheTower_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function TheTower_use(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host).value
    if roll == 1 or roll == 2 then
        local players = getPlayers(host)
        for _, player in ipairs(players) do
            dealDamage(host, PLAYER, owner.id, PLAYER, player.id, 1)
        end
        return
    end
    if roll == 3 or roll == 4 then
        local monsters = getActiveMonsters(host)
        for _, monster in ipairs(monsters) do
            dealDamage(host, PLAYER, owner.id, MONSTER, monster.id, 1)
        end
        return
    end
    if roll == 5 or roll == 6 then
        local players = getPlayers(host)
        for _, player in ipairs(players) do
            dealDamage(host, PLAYER, owner.id, PLAYER, player.id, 2)
        end
        return
    end
end