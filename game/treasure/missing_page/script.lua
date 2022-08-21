function MissingPage_trigger_check(host, me)
    return Common_LastRoll(host, me)["value"] == 5
end

function MissingPage_trigger(host)

    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner["id"], "Force player to reroll?", {"Yes", "No"})
    if choice == "No" then
        return
    end
    local rollI = Common_RollStackSize(host)
    local value = math.random(1, 6)
    setRollValue(host, rollI-1, value)
end