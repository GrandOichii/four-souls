--  TODO untested

function SpoonBender_cost(host, cardInfo)
    return Common_TargetRoll(host, cardInfo["ownerID"]) and Common_Tap(host)
end

function SpoonBender_tap(host)
    local target = popTarget(host)
    local rolls = getRollStack(host)
    local roll = rolls[target["id"]+1]
    if roll["value"] == 6 then
        return
    end
    setRollValue(host, target["id"], roll["value"] + 1)
end