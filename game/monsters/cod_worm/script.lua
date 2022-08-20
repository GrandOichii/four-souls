function CodWorm_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	--  TODO
	addCoins(host, killer.id, 3)
end