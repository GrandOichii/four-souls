function DankGlobin_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 2)
end

function DankGlobin_death_cost(host, cardInfo)
	local killerID = getLastKillerID(host)
	return Common_TargetPlayer(host, killerID)
end

function DankGlobin_death(host)
	local target = popTarget(host)
	Common_Discard(host, target.id, 2)
end