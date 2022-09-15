function TheFallen_rewards(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end

function TheFallen_death(host)
    local owner = getTopOwner(host)
    Common_OrderTopCards(host, owner.id, MONSTER_DECK, 5)
end
