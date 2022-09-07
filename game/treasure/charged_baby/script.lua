function ChargedBaby_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 2
end

function ChargedBaby_trigger_cost(host, me)
    return Common_TargetTappedCard(host, me.ownerID)
end

function ChargedBaby_trigger(host)
    local target = popTarget(host)
    local card = cardWithID(host, target.id)
    local owner = getTopOwner(host)
    --  TODO change text to display card name
    local choice = requestSimpleChoice(host, owner.id, 'Recharge ${'..card.name..'}?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    rechargeCard(host, target.id)
end