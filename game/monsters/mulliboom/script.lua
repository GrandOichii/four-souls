function Mulliboom_rewards(host, me, killer)
	addCoins(host, killer.id, 6)
end

function Mulliboom_death(host)
	local p = getCurrentPlayer(host)
	local choice = Common_ChoosePlayer(host, p.id)
	dealDamage(host, PLAYER, p.id, PLAYER, choice, 3)
end