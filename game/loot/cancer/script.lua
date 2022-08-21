function Cancer_trigger_check(host, me)
    local lr = Common_LastRoll(host, me)
    local owner = getOwner(host, me.id)
    return lr.ownerID == owner.id and Common_OncePerTurn(host, me.id)
end

function Cancer_trigger(host)
    Common_ModLastRoll(host, 1)
end
