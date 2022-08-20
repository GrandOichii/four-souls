function LuckyFoot_cost(host, cardInfo)
    return Common_TargetNonCombatRoll(host, cardInfo.ownerID)
end

function LuckyFoot_tap(host)
    local target = popTarget(host)
    local roll = Common_RollWithID(host, target.id)
    if roll == 6 then
        return
    end
    if roll == 5 then
        setRollValue(host, target.id, 6)
        return
    end
    local owner = getTopOwner(host)
    local response = requestSimpleChoice(host, owner.id, 'Choose what to do with the dice roll', {'+1', '+2'})
    if response == '+1' then
        setRollValue(host, target.id, roll.value + 1)
        return
    end
    setRollValue(host, target.id, roll.value + 2)
end