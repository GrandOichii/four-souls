function PeepingFatty_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
end

function PeepingFatty_rewards(host)
    local owner = getTopOwner(host)
    Common_OrderTopCards(host, owner.id, LOOT_DECK, 5)
end
