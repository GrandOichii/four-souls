function CambionConception_trigger_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function CambionConception_trigger(host)
    local damageEvent = getDamageEvent(host)
    local me = this(host)
    local owner = getTopOwner(host)
    addCounters(host, me.id, damageEvent.amount)
    me = this(host)
    if me.counters >= 6 then
        removeCounters(host, me.id, 6)
        gainTreasure(host, owner.id, 1)
    end
end