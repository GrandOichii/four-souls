function FannyPack_trigger_check(host, me)
    local owner = getOwner(host, me["id"])
    local damageEvent = getDamageEvent(host)
    return damageEvent["type"] == PLAYER and damageEvent["id"] == owner["id"]
    -- return damageEvent["type"] == PLAYER
end

function FannyPack_trigger(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    lootCards(host, owner["id"], 1)
end