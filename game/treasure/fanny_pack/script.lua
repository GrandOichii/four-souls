function FannyPack_trigger_check(host, me)
    return Common_OwnerDamaged(host, me["id"])
end

function FannyPack_trigger(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 1)
end