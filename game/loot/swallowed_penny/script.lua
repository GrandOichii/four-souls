function SwallowedPenny_trigger_check(host, me)
    return Common_OwnerDamaged(host, me["id"])
end

function SwallowedPenny_trigger(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 1)
end