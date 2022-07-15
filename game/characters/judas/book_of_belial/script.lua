function BookOfBelial_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo["ownerID"]) and Common_Tap(host)
end

function BookOfBelial_tap(host)
    local target = popTarget(host)
    local rolls = getRollStack(host)
    local roll = rolls[target["id"]+1]
    if roll == 6 then
        setRollValue(host, target["id"], roll["value"] - 1)
        return
    end
    if roll == 1 then
        setRollValue(host, target["id"], roll["value"] + 1)
        return
    end
    local owner = getTopOwner(host)
    local response = requestSimpleChoice(host, owner["id"], "Choose what to do with the dice roll", {"+1", "-1"})
    if response == "+1" then
        setRollValue(host, target["id"], roll["value"] + 1)
        return
    end
    setRollValue(host, target["id"], roll["value"] - 1)
end