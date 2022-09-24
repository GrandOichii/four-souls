function PHD_trigger_check(host, me)
    local roll = Common_LastRoll(host, me)
    return roll.ownerID == me.ownerID and not roll.isCombatRoll
end

function PHD_trigger(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Modify roll?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    local mod = 1
    choice = requestSimpleChoice(host, owner.id, 'Choose what to do with the roll', {'+1', '-1'})
    if choice == '-1' then
        mod = -1
    end
    Common_ModLastRoll(host, mod)
end
