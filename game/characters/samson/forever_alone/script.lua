function ForeverAlone_cost(host, cardInfo)
    return Common_Tap(host)
end

function ForeverAlone_tap(host)
    --  TODO
end

function ForeverAlone_trigger_check(host, me)
    return Common_OwnerDamaged(host, me["id"])
end

function ForeverAlone_trigger(host)
    local _ = getDamageEvent(host)
    local card = this(host)
    rechargeCard(host, card["id"])
end