function Mulligan_rewards(host, me, killer)
	addCoins(host, killer["id"], 3)
end

function Mulligan_death(host)
	expandActiveMonsters(host, 1)
end