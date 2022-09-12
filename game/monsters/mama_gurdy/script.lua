function MamaGurdy_rewards(host)
    local roll = popRollStack(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, roll.value)
end