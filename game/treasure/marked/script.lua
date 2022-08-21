function Marked_trigger_check(host, me)
    local lr = Common_LastRoll(host, me)
    local owner = getOwner(host, me.id)
    return lr.ownerID == owner.id  and lr.isCombatRoll and Common_OncePerTurn(host, me.id)
end

function Marked_trigger(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, 'Change roll to a 5?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    local rs = getRollStack(host)
    local roll = rs[#rs]
    setRollValue(host, #rs-1, 5)
end