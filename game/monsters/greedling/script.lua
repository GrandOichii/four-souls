function Greedling_rewards(host, me, killer)
	addCoins(host, killer["id"], 7)
end

function Greedling_death(host)
	local killerID = getLastKillerID(host)
	local choice = Common_ChoosePlayer(host, killerID)
	local p = Common_PlayerWithID(host, choice)
	subCoins(host, choice, math.min(p.coins, 7))
end