function EyeOfGreed_trigger_check(host)
    local rollEvent = getLastRoll(host)
    return rollEvent["amount"] == 5
end

function EyeOfGreed_trigger(host)

    local owner = getOwner(host)
    addCoins(host, owner["id"], 3)

end