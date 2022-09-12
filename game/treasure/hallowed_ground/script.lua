function HallowedGround_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 2
end

function HallowedGround_trigger(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function HallowedGround_cost(host, info)
    return Common_RemoveCounters(host, 3)
end

function HallowedGround_effect(host)
    local owner = getTopOwner(host)
    local discard = getDiscard(host, LOOT_DECK)
    if #discard == 0 then
        return
    end
    local last = discard[#discard]
    removeFromEverywhere(host, last.id)
    moveToHand(host, owner.id, last.id)
end
