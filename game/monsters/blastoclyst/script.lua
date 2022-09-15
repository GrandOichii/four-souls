function Blastoclyst_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
end

function Blastoclyst_death(host)
	--  TODO replace with prompt to attack
    local player = getCurrentPlayer(host)
    expandActiveMonsters(host, 2)
	addAttackOpportunity(host, player.id, false, true, {-1})

end
