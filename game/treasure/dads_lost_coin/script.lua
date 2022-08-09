function DadsLostCoin_trigger_check(host, me)
    return Common_LastRoll(host).value == 1
end

function DadsLostCoin_trigger(host)

    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, "Force player to reroll?", {"Yes", "No"})
    if choice == "No" then
        return
    end
    local rollI = Common_RollStackSize(host)
    setRollValue(host, rollI-1, math.random(1, 6))
end