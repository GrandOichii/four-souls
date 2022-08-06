function Greed_rewards(host, me, killer)
	addCoins(host, killer.id, 9)
	addSouls(host, killer.id, 1)
end

function Greed_trigger_check(host, me)
	return Common_MonsterDealtCombatDamage(host, me.id)
end

function Greed_trigger(host)
    local _ = getDamageEvent(host)
	local players = getPlayers(host)
	for _, player in ipairs(players) do
		subCoins(host, player.id, math.min(4, player.coins))
	end
end
