function GuppysHairball_trigger_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function GuppysHairball_trigger_cost(host, cardInfo)
    return Common_Roll(host, cardInfo.ownerID)
end

function GuppysHairball_trigger(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host).value
    if roll == 6 then
        addBlueHealth(host, owner.id, 1)
    end
end