function CursedPsyHorf_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	lootCards(host, killer.id, 2)
end

function CursedPsyHorf_trigger_check(host)
	return true
end

function CursedPsyHorf_trigger(host)
	local activations = getActivations(host)
	local activation = activations[#activations]
	local me = this(host)
	dealDamage(host, MONSTER, me.id, PLAYER, activation.ownerID, 1)
end
