function CursedDople_trigger_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function CursedDople_trigger(host)
    local me = this(host)
    local cPlayer = getCurrentPlayer(host)
    for _, player in ipairs(getPlayers(host)) do
        if player.id ~= cPlayer.id then
            dealDamage(host, MONSTER, me.id, PLAYER, player.id, 1)
        end
    end
end

function CursedDople_rewards(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host)
    addCoins(host, owner.id, roll.value)
end
