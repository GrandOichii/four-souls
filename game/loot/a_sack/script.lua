-- function BlankRune_use(host)

--     local function dealDamageF(id, amount)
--         dealDamage(host, PLAYER, id, amount)
--     end

--     local d = {
--         addCoins,
--         lootCards,
--         dealDamageF,
--         addCoins,
--         lootCards,
--         addCoins
--     }

--     local n = math.random(6)
--     local players = getPlayers(host)
--     local action = d[n]
--     for i, player in ipairs(players) do
--         action(host, player["id"], n)
--     end

-- end

function ASack_use(host)
    local owner = getOwner(host)
    lootCards(host, owner["id"], 3)
end