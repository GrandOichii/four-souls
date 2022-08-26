--  TODO untested

function Godhead_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo.ownerID)
end

function Godhead_tap(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    local response = requestSimpleChoice(host, owner.id, "Set roll to:", {"1", "6"})
    if response == "1" then
        setRollValue(host, target.id, 1)
        return
    end
    setRollValue(host, target.id, 6)
end