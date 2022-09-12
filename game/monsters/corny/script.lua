function Corny_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 4)
end

function Corny_death(host)
    local me = this(host)
    local cPlayer = getCurrentPlayer(host)
    local players = getPlayers(host)
    local ids = {}
    for _, player in ipairs(players) do
        if player.id ~= cPlayer.id then
            ids[#ids+1] = player.id
        end
    end
    local id = ids[math.random(1, #ids)]
    dealDamage(host, MONSTER, me.id, PLAYER, id, 2)
end
