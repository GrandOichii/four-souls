function SackOfPennies_trigger_check(host)
    --  TODO
    local rollEvent = getLastRoll(host)
    return rollEvent["amount"] == 1
end


function SackOfPennies_trigger(host)
    --  TODO
    local owner = getTopOwner(host)
    local choice = requestChoice(host, owner["id"], "Recharge Sack of Pennies?", "Yes|No")
    if (choice == "No") then
        return
    end
    local id = this(host)["id"]
    rechargeCard(host, id)

end

function SackOfPennies_cost(host, cardInfo)
    return Common_Tap(host, cardInfo)
end

function SackOfPennies_tap(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 1)

end