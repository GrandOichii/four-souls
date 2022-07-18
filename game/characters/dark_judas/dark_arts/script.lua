function DarkArts_roll_trigger_check(host, me)
    return Common_LastRoll(host)["value"] == 6
end

function DarkArts_roll_trigger(host)
    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 3)
end

function DarkArts_death_trigger_check(host, me)
    local owner = getOwner(host, me["id"])
    return Common_OpponentDied(host, owner["id"])
end

function DarkArts_death_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 2)
end