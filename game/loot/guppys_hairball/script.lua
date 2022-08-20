function GuppysHairball_trigger_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function GuppysHairball_trigger(host)
    local owner = getTopOwner(host)
    local roll = popRollStack(host).value
    if roll == 6 then
        addBlueHealth(host, owner.id, 1)
    end
end