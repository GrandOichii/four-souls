function ThePoop_trigger_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function ThePoop_trigger(host)
    local _ = getDamageEvent(host)
    local card = this(host)
    addCounters(host, card.id, 1)
end

function ThePoop_cost(host, cardInfo)
    return Common_RemoveCounter(host)
end

function ThePoop_activate(host)
    local owner = getTopOwner(host)
    addBlueHealth(host, owner.id, 1)
end