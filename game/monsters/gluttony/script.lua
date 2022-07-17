function Gluttony_rewards(host, me, killer)
	lootCards(host, killer["id"], 1)
	addSouls(host, killer["id"], 1)
end