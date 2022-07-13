function DadsLostCoin_trigger_check(host, me)
    return Common_LastRoll(host)["value"] == 1
end

function DadsLostCoin_trigger(host)
    --  TODO prompt with yes/no
    local rollI = Common_RollStackSize(host)
    setRollValue(host, rollI, math.random(1, 6))
end