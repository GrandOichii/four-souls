function Envy_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 1)
end

function Envy_death(host)
	local player = getCurrentPlayer(host)
	Common_IncAttackCount(host, player.id, true)
end