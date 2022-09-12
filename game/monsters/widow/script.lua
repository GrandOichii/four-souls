function Widow_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
end
