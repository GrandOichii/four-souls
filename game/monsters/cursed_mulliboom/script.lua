function CursedMulliboom_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 2)
end

function CursedMulliboom_trigger_check(host, me)
    return Common_LastDeath(host).type == MONSTER
end

function CursedMulliboom_trigger(host)
    local players = getPlayers(host)
    local me = this(host)
    local player = players[math.min(1, #players)]
    dealDamage(host, MONSTER, me.id, PLAYER, player.id, 3)
end
