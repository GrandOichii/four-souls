function BookOfBelial_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID) and Common_Tap(host)
end

function BookOfBelial_tap(host)
    local target = popTarget(host)
    local roll = Common_RollWithID(host, target.id)
    if roll == 6 then
        setRollValue(host, target.id, roll.value - 1)
        return
    end
    if roll == 1 then
        setRollValue(host, target.id, roll.value + 1)
        return
    end
    local owner = getTopOwner(host)
    local response = requestSimpleChoice(host, owner.id, 'Choose what to do with the dice roll', {'+1', '-1'})
    if response == '+1' then
        setRollValue(host, target.id, roll.value + 1)
        return
    end
    setRollValue(host, target.id, roll.value - 1)
end


function BookOfBelial_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function BookOfBelial_trigger(host)
    local me = this(host)
    rechargeCard(host, me.id)
end
