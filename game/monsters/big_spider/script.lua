function BigSpider_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	--  TODO
	lootCards(host, killer["id"], 1)
end