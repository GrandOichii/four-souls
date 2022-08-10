function TheD6_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID) and Common_Tap(host)
end

function TheD6_tap(host)
    local target = popTarget(host)
    local value = math.random(1, 6)
    setRollValue(host, target.id, value)
end

function TheD6_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TheD6_trigger(host)
    local me = this(host)
    rechargeCard(host, me.id)
end
