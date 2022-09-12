function CursedTumor_rewards(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    addCoins(host, owner.id, roll.value)
end

function CursedTumor_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 4
end

function CursedTumor_trigger(host)
    local me = this(host)
    for _, player in ipairs(getPlayers(host)) do
        dealDamage(host, MONSTER, me.id, PLAYER, player.id, 1)
    end
end
