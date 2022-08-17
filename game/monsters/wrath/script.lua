function Wrath_death_cost(host, info)
	return Common_Roll(host, info.ownerID)
end

function Wrath_death(host)
	local roll = getLastRoll(host)
	local me = this(host)
	local amount = 1
	if roll.value > 3 then
		amount = 2
	end
	local players = getPlayers(host)
	for _, player in ipairs(players) do
		dealDamage(host, MONSTER, me.id, PLAYER, player.id, amount)
	end
end

function Wrath_rewards(host, me, killer)
	addCoins(host, killer.id, 6)
end
