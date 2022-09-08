function Conquest_rewards(host)
	local killer = getTopOwner(host)
	addCoins(host, killer.id, 6)
end

function Conquest_death(host)
	local player = getCurrentPlayer(host)
	Common_IncAttackCount(host, player.id, true)
end