function Mom_rewards(host, me, killer)
    gainTreasure(host, killer.id, 1)
	addSouls(host, killer["id"], 2)
end