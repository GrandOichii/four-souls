function BoomFly_rewards(host, me, killer)
	addCoins(host, killer.id, 4)
end

function BoomFly_death(host)
	local players = getPlayers(host)
	local me = this(host)
	for _, player in ipairs(players) do
		dealDamage(host, MONSTER, me.id, PLAYER, player.id, 1)
	end
end