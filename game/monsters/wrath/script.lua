function Wrath_death(host)
	local roll = popRollStack(host)
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

function Wrath_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer.id, 6)
end
