function Wrath_rewards(host, me, killer)
	addCoins(host, killer["id"], 6)
	addSouls(host, killer["id"], 1)
end