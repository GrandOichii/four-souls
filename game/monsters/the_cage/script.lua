function TheCage_rewards(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 2)
end
