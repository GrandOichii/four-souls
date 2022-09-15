function Swarmer_rewards(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end

function Swarmer_death(host)
    expandActiveMonsters(host, 2)
end
