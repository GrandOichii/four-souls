function TelepathyForDummies_trigger1_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TelepathyForDummies_trigger1_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function TelepathyForDummies_trigger1(host)
    local roll = getLastRoll(host)
    local me = this(host)
    CardData[me.id] = {
        lr = roll.value
    }
end

function TelepathyForDummies_trigger2_check(host, me)
    if CardData[me.id] == nil then
        return false
    end
    if CardData[me.id].lr == nil then
        return false
    end
    return Common_LastRoll(host).ownerID == me.ownerID and Common_OncePerTurn(host, me.id)
end

function TelepathyForDummies_trigger2(host)
    local owner = getTopOwner(host)
    local lr = Common_LastRoll(host)
    local choice = requestSimpleChoice(host, owner.id, 'Change the result to '..lr.value..'?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    local rs = getRollStack(host)
    local roll = rs[#rs]
    if roll.value == 6 then
        return
    end
    local me = this(host)
    setRollValue(host, #rs-1, CardData[me.id].lr)
end

function TelepathyForDummies_trigger3_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TelepathyForDummies_trigger3(host)
    local me = this(host)
    if CardData[me.id] == nil then
        CardData[me.id] = {}
    end
    if CardData[me.id].lr ~= nil then
        CardData[me.id].lr = nil
    end
end