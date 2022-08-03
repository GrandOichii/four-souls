function TheresOptions_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function TheresOptions_trigger(host)
    local owner = getTopOwner(host)
    incPurchaseAmount(host, owner.id)
end

function TheresOptions_cost(host)
    return true
end

function TheresOptions_peek(host)
    --  TODO
end
