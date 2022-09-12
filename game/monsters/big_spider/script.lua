function BigSpider_rewards(host)
	local killer = getTopOwner(host)
	lootCards(host, killer.id, 1)
end

function BigSpider_death(host)
	local player = getCurrentPlayer(host)
	addAttackOpportunity(host, player.id, false, true, {-1})
end