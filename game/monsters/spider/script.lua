function Spider_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
    lootCards(host, killer["id"], 1)
end