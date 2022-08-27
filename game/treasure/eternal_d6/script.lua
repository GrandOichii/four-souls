function EternalD6_effect1(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function EternalD6_cost2(host, info)
    return Common_RemoveCounters(host, 2)
end

function EternalD6_effect2(host)
    CardData[this(host).id].activated = true
end

function EternalD6_trigger1_check(host, me)
    return true
end

function EternalD6_trigger1(host)
    CardData[this(host).id].activated = false
end

function EternalD6_enter(host)
    local me = this(host)
    CardData[me.id] = {
        activated = false
    }
end

function EternalD6_trigger2_check(host, me)
    local lr = Common_LastRoll(host, me).value
    return CardData[me.id].activated and (lr == 3 or lr == 2)
end

function EternalD6_trigger2(host)
    local rollI = Common_RollStackSize(host)
    setRollValue(host, rollI-1, 1)
end
