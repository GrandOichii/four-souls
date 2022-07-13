function BloodPenny_trigger(host)

    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 1)

end

function BloodPenny_trigger_check(host)

    local deathEvent = getLastDeath(host)
    return deathEvent["type"] == PLAYER

end