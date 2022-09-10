function CurseOfAmnesia_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function CurseOfAmnesia_trigger(host)
    local owner = getTopOwner(host)
    Common_Discard(host, owner.id, 2)
end
