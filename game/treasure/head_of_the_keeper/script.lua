function HeadOfTheKeeper_trigger1_check(host, me)
    local owner = getOwner(host, me.id)
    local damageEvent = getTopDamageEvent(host)
    if damageEvent.sourceType ~= PLAYER then
        return false
    end
    if damageEvent.sourceID ~= owner.id then
        return false
    end
    return true
end

function HeadOfTheKeeper_trigger1(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 1)
end

function HeadOfTheKeeper_trigger2_check(host, me)
    return getLastKillerID(host) == me.ownerID
end

function HeadOfTheKeeper_trigger2(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 2)
end