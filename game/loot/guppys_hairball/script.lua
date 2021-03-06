function GuppysHairball_trigger_check(host, me)
    return Common_OwnerDamaged(host, me["id"])
end

function GuppysHairball_trigger_cost(host, me)
    return Common_Roll(host, me["ownerID"])
end

function GuppysHairball_trigger(host)
    local _ = getDamageEvent(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host)["value"]
    if roll == 6 then
        addBlueHealth(host, owner["id"], 1)
    end
end