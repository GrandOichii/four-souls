function CambionConception_trigger_check(host, me)
    return Common_OwnerDamaged(host, me["id"])
end

function CambionConception_trigger(host)
    local _ = getDamageEvent(host)
    local card = this(host)
    addCounters(host, card["id"], 1)
    card = this(host)
    if card["counters"] == 6 then
        removeCounters(host, card["id"], 6)
        plusOneTreasure(host, card["ownerID"])
    end
end