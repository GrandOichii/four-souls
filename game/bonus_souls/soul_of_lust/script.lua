function SoulOfLust_trigger_check(host)
    return Common_LastDeath(host).type == MONSTER
end

function SoulOfLust_trigger(host)
    local me = this(host)
    addCounters(host, me.id, 1)
    me = this(host) --  TODO? may cause errors
    if me.counters >= 6 then
        pushToStack(host, 'SoulOfLust_gain', getCurrentPlayer(host).id, me.id, 'trigger')
    end
end

function SoulOfLust_gain(host)
    local owner = getTopOwner(host)
    local me = this(host)
    removeFromEverywhere(host, me.id)
    resetCounters(host, me.id)
    addSoulCard(host, owner.id, me.id)
end