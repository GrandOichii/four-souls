--  TODO untested

function TheMagician_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID)
end

function TheMagician_use(host)
    discardMe(host, this(host).id)

    local target = popTarget(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Set dice result to what?', {'1', '2', '3', '4', '5', '6'})
    setRollValue(host, target.id, tonumber(choice))

end
