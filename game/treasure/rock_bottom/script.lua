function RockBottom_enter(host)
	local owner = getTopOwner(host)
	local me = this(host)
    addCounters(host, me.id, #owner.hand)
end

function RockBottom_trigger_check(host, me)
    local owner = getOwner(host, me.id)
    return Common_OwnersTurn(host, me.id) and #owner.hand < me.counters
end

function RockBottom_trigger(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
end
