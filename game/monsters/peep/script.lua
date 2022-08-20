function Peep_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    gainTreasure(host, killer.id, 1)
end