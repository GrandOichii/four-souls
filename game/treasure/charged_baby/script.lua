function ChargedBaby_trigger_check(host, me)
    return Common_LastRoll(host).value == 2
end

function ChargedBaby_trigger_cost(host, me)
    return Common_TargetTappedCard(host, me.ownerID)
end

function ChargedBaby_trigger(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    --  TODO change text to display card name
    local choice = requestSimpleChoice(host, owner.id, 'Recharge item?', {'Yes', 'No'})
    rechargeCard(host, target.id)
end