function CambionConception_trigger_check(host, me)
    local owner = getOwner(host, me["id"])
    local damageEvent = getDamageEvent(host)
    return damageEvent["type"] == PLAYER and damageEvent["id"] == owner["id"]
end

function CambionConception_trigger(host)
    local card = this(host)
    addCounters(host, card["id"], 1)
    card = this(host)
    if card["counters"] == 6 then
        removeCounters(host, card["id"], 6)
        plusOneTreasure(host, card["ownerID"])
    end
end