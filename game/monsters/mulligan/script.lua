function Mulligan_rewards(host)
	local killerID = getLastKillerID(host)
	local killer = Common_PlayerWithID(host, killerID)
	addCoins(host, killer["id"], 3)
end

function Mulligan_death(host)
	expandActiveMonsters(host, 1)
end