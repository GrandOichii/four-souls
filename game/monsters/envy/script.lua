function Envy_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 1)
end

function Envy_death(host)
	--  TODO replace with prompt to attack

	local player = getCurrentPlayer(host)
	Common_IncAttackCount(host, player.id, true)
end