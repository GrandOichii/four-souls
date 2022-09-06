function SoulOfWrath_trigger_check(host, me)
    return Common_LastDeath(host).type == PLAYER
end

function SoulOfWrath_trigger(host)
    local me = this(host)
    addCounters(host, me.id, 1)
    me = this(host)
    if me.counters >= 6 then
        pushToStack(host, 'SoulOfWrath_gain', getCurrentPlayer(host).id, me.id, 'trigger')
    end
end

function SoulOfWrath_gain(host)
    local owner = getTopOwner(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    resetCounters(host, me.id)
    addSoulCard(host, owner.id, me.id)
end