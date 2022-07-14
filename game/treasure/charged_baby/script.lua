function ChargedBaby_trigger_check(host, me)
    return Common_LastRoll(host)["value"] == 2 and Common_TargetTappedCard(host, me["ownerID"])
end

function ChargedBaby_trigger(host)
    local target = popTarget(host)
    rechargeCard(host, target["id"])
end