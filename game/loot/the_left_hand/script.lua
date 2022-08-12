function TheLeftHand_trigger_check(host, cardInfo)
    local death = Common_LastDeath(host)
    return death.type == PLAYER and death.id ~= cardInfo.ownerID
end

function TheLeftHand_trigger(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Recharge an item?', {'Yes', 'No'})
    if choice == 'No' then return end
    Common_RechargeAnItem(host, owner.id, false)
end