function TheBloat_rewards(host, me, killer)
    gainTreasure(host, killer.id, 1)
end

function TheBloat_trigger_check(host, me)
    return Common_MonsterDealtCombatDamage(host, me.id)
end

function TheBloat_trigger(host)
    local _ = getDamageEvent(host)
    local players = getPlayers(host)
    local cPlayer = getCurrentPlayer(host)
    local me = this(host)
    for _, player in ipairs(players) do
        if player.id ~= cPlayer.id then
            dealDamage(host, MONSTER, me.id, PLAYER, player.id, 1)
        end
    end
end
