function BloodPenny_trigger_check(host, me)
    return Common_LastDeath(host)["type"] == PLAYER
end

function BloodPenny_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 1)
end
