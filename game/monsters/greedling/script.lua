function Greedling_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 7)
end

function Greedling_death_cost(host, info)
	return Common_TargetPlayer(host, info.ownerID)
end

function Greedling_death(host)
	local choice = popTarget(host).id
	local p = Common_PlayerWithID(host, choice)
	subCoins(host, choice, math.min(p.coins, 7))
end