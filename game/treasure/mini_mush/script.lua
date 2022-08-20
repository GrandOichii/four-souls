function MiniMush_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID)
end

function MiniMush_tap(host)
    local target = popTarget(host)
    local roll = Common_RollWithID(host, target.id)
    if roll == 1 then
        return
    end
    if roll == 2 then
        setRollValue(host, target.id, 1)
        return
    end
    local owner = getTopOwner(host)
    local response = requestSimpleChoice(host, owner.id, 'Choose what to do with the dice roll', {'-1', '-2'})
    if response == '-1' then
        setRollValue(host, target.id, roll.value - 1)
        return
    end
    setRollValue(host, target.id, roll.value - 2)
end