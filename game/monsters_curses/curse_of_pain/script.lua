function CurseOfPain_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function CurseOfPain_trigger(host)
    local owner = getTopOwner(host)
    dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 1)
end
