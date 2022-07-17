function Famine_rewards(host, me, killer)
	lootCards(host, killer["id"], 3)
	addSouls(host, killer["id"], 1)
end