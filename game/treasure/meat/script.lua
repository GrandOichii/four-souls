function Meat_trigger_check(host, me)
    local lr = Common_LastRoll(host, me)
    local owner = getOwner(host, me.id)
    return lr.ownerID == owner.id and lr.isCombatRoll
end

function Meat_trigger(host)
    Common_ModLastRoll(host, 1)
end
