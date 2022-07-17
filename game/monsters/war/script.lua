function War_rewards(host, me, killer)
	addCoins(host, killer["id"], 8)
	addSouls(host, killer["id"], 1)
end