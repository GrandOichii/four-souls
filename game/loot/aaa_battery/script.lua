function AAABattery_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end


function AAABattery_trigger(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll > 3 then
        Common_RechargeAnItem(host, owner.id, false)
    end
end