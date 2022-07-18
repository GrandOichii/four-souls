--  TODO untested

function SacredHeart_trigger_check(host, me)
    return Common_OwnerRolled(host, me["ownerID"], 1)
end

function SacredHeart_trigger(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner["id"], "Turn roll into 6?", {"Yes", "No"})
    if choice == "No" then
        return
    end
    local rollI = Common_RollStackSize(host)
    setRollValue(host, rollI-1, 6)
end