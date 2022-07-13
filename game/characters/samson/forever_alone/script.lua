function ForeverAlone_cost(host, cardInfo)
    return Common_Tap(host)
end

function ForeverAlone_tap(host)
    --  TODO
end

function ForeverAlone_trigger_check(host, me)
    local owner = getOwner(host, me["id"])
    local damageEvent = getDamageEvent(host)
    return damageEvent["type"] == PLAYER and damageEvent["id"] == owner["id"]
end

function ForeverAlone_trigger(host)
    local _ = getDamageEvent(host)
    local card = this(host)
    rechargeCard(host, card["id"])
end