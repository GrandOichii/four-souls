function BlackBony_rewards(host, me, killer)
	--  TODO
	lootCards(host, killer.id, math.random(1, 6))
end

function BlackBony_death(host)
	local killerID = getLastKillerID(host)
	local me = this(host)
	dealDamage(host, MONSTER, me.id, PLAYER, killerID, 1)
end