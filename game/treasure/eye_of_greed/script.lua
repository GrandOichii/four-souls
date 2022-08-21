function EyeOfGreed_trigger_check(host)
    return Common_LastRoll(host, me)["value"] == 5
end

function EyeOfGreed_trigger(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 3)

end