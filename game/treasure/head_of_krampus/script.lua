--  TODO untested

function HeadOfKrampus_cost(host, cardInfo)
    return Common_Tap(host) and Common_Roll(host, cardInfo.ownerID)
end

function HeadOfKrampus_tap(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host).value
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