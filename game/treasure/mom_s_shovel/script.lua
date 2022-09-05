function MomsShovel_enter(host)
    local me = this(host)
    tapCard(host, me.id)
end

function MomsShovel_cost(host, info)
    return Common_TargetAnySoul(host, info.ownerID)
end

function MomsShovel_effect(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    removeFromEverywhere(host, target.id)
    addSoulCard(host, owner.id, target.id)
end
