function Tumor_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 6)
end

function Tumor_trigger_check(host, me)
    return Common_MonsterDamaged(host, me.id)
end

function Tumor_trigger(host)
    local me = this(host)
    local aPlayer = getCurrentPlayer(host)
    for _, player in ipairs(getPlayers(host)) do
        if player.id ~= aPlayer.id then
            dealDamage(host, MONSTER, me.id, PLAYER, player.id, 1)
        end
    end
end
