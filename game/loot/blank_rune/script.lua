function BlankRune_use(host)
    discardMe(host, this(host).id)
    local owner = getTopOwner(host)
    local dealDamageF = function(host_, targetID, amount)
        dealDamage(host_, NO_SOURCE, NO_SOURCE_ID, PLAYER, targetID, amount)
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
        action(host, player.id, roll)
    end
    

end