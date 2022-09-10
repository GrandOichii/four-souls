function CurseOfTheHollow_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function CurseOfTheHollow_trigger(host)
    local owner = getTopOwner(host)
    local amount = Common_SoulCount(owner)
    Common_Discard(host, owner.id, amount)
    amount = math.min(amount, owner.coins)
    subCoins(host, owner.id, amount)
end
