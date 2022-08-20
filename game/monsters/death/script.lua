function Death_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    gainTreasure(host, killer.id, 1)
end

function Death_death_cost(host, info)
    return Common_TargetPlayer(host, info.ownerID)
end

function Death_death(host)
    local target = popTarget(host).id
    killEntity(host, PLAYER, target)
end