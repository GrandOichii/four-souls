function TheHaunt_rewards(host, me, killer)
	plusOneTreasure(host, killer["id"])
	addSouls(host, killer["id"], 1)
end