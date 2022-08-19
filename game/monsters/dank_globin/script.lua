function DankGlobin_rewards(host, me, killer)
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