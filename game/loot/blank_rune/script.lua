function BlankRune_use(host)
    local owner = getTopOwner(host)
    local dealDamageF = function(host, targetID, amount)
        dealDamage(host, PLAYER, owner["id"], PLAYER, targetID, amount)
    end

    local d = {
        addCoins,
        lootCards,
        dealDamageF,
        addCoins,
        lootCards,
        addCoins
    }

    local roll = popRollStack(host).value
    local players = getPlayers(host)
    local action = d[roll]
    for _, player in ipairs(players) do
        action(host, player["id"], roll)
    end

end