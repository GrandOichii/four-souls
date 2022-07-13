function ChargedBaby_trigger_check(host, me)
    return Common_LastRoll(host)["value"] == 2
end

function ChargedBaby_trigger(host)
    local owner = getTopOwner(host)
    Common_TargetTappedCard(host, owner["id"])
    local target = popTarget(host)
    rechargeCard(host, target["id"])
end