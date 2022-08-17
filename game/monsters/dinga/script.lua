function Dinga_rewards_cost(host, info)
	return Common_Roll(host, info.ownerID)
end

function Dinga_rewards(host, me, killer)
	local roll = getLastRoll(host)
	addCoins(host, killer.id, roll.value)
end