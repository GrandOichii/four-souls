function UltraFleshKid_trigger1_check(host, me)
    return true
end

function UltraFleshKid_trigger1(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function UltraFleshKid_trigger2_check(host, me)
    return Common_OwnersTurn(host, me.id) and me.counters >= 12
end

function UltraFleshKid_trigger2(host)
    local me = this(host)
    local owner = getTopOwner(host)
    destroyCard(host, me.id)
    addSouls(host, owner.id, 1)
end
