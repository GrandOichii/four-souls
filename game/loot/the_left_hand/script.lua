function TheLeftHand_trigger_check(host, cardInfo)
    local death = Common_LastDeath(host)
    return death.type == PLAYER and death.id ~= cardInfo.ownerID
end

function TheLeftHand_trigger(host)
    local owner = getTopOwner(host)
    Common_RechargeAnItem(host, owner.id, false)
end