function CurseOfGreed_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function CurseOfGreed_trigger(host)
    local owner = getTopOwner(host)
    local amount = math.min(4, owner.coins)
    subCoins(host, owner.id, amount)
end
