function JudasShadow_trigger_check(host, me)
    return Common_OwnerDied(host, me.ownerID)
end

function JudasShadow_trigger(host)
    local me = this(host)
    addCounters(host, me.id, 2)
end

function JudasShadow_leave(host)
    local me = this(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, me.counters)
    lootCards(host, owner.id, me.counters)
end
