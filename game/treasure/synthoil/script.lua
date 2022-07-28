function Synthoil_trigger_check(host, me)
    local lr = Common_LastRoll(host)
    local owner = getOwner(host, me.id)
    return lr.ownerID == owner.id and lr.isCombatRoll
end

function Synthoil_trigger(host)
    Common_ModLastRoll(host, 1)
end
