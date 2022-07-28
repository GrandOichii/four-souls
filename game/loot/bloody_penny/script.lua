function BloodyPenny_trigger_check(host, me)
    return Common_LastDeath(host).type == PLAYER
end

function BloodyPenny_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 1)
end
