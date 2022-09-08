function Portal_rewards(host)
	local killer = getTopOwner(host)
	addCoins(host, killer.id, 3)
end

function Portal_death(host)
	local player = getCurrentPlayer(host)
	Common_IncAttackCount(host, player.id, true)
end