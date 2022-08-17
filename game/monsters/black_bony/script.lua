function BlackBony_rewards_cost(host, info)
	return Common_Roll(host, info.ownerID)
end

function BlackBony_rewards(host, me, killer)
	local roll = getLastRoll(host)
	lootCards(host, killer.id, roll.value)
end

function BlackBony_death(host)
	local killerID = getLastKillerID(host)
	local me = this(host)
	dealDamage(host, MONSTER, me.id, PLAYER, killerID, 1)
end