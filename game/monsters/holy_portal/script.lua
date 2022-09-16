function HolyPortal_rewards(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 5)
end

function HolyPortal_trigger_check(host, me)
    return Common_LastDeath(host).type == MONSTER
end

function HolyPortal_trigger(host)
    local player = getCurrentPlayer(host)
    Common_IncAttackCount(host, player.id, false)
end
