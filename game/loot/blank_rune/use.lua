local function dealDamageF(id, amount)
    dealDamage(PLAYER, id, amount)
end

d = {
    addCoins,
    lootCards,
    dealDamageF,
    addCoins,
    lootCards,
    addCoins
}

local n = math.random(6)
local players = getPlayers()
local action = d[n]
for i, player in ipairs(players) do
    action(player["id"], n)
end
