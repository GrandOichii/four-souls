function BlackBony_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	local roll = popRollStack(host)
	lootCards(host, killer.id, roll.value)
end

function BlackBony_death(host)
	local killerID = getLastKillerID(host)
	local me = this(host)
	dealDamage(host, MONSTER, me.id, PLAYER, killerID, 1)
end