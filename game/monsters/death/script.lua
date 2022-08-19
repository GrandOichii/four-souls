function Death_rewards(host, me, killer)
    gainTreasure(host, killer.id, 1)
end

function Death_death_cost(host, info)
    return Common_TargetPlayer(host, info.ownerID)
end

function Death_death(host)
    local target = popTarget(host).id
    killEntity(host, PLAYER, target)
end