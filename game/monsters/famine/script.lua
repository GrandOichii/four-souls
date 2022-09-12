function Famine_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 3)
end

function Famine_death(host)
	local player = getCurrentPlayer(host)
	incSkipCounter(host, player.id)
end