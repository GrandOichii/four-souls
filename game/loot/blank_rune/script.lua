function BlankRune_cost(host, cardInfo)
    return Common_Roll(host, cardInfo["ownerID"])
end

function BlankRune_use(host)

    local d = {
        addCoins,
        lootCards,
        dealDamage,
        addCoins,
        lootCards,
        addCoins
    }

    local roll = getLastRoll(host)["value"]
    local players = getPlayers(host)
    local action = d[roll]
    for _, player in ipairs(players) do
        action(host, player["id"], roll)
    end

end