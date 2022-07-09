function SackOfPennies_trigger_check(host)
    local rollEvent = getLastRoll(host)
    return rollEvent["amount"] == 1
end


function SackOfPennies_trigger(host)

    local owner = getOwner(host)
    local choice = requestChoice(host, owner["id"], "Recharge Sack of Pennies?", "Yes|No")
    if (choice == "No") then
        os.exit()
    end
    local id = this(host)["id"]
    rechargeCard(host, id)

end

function SackOfPennies_tap(host)

    local owner = getOwner(host)
    addCoins(host, owner["id"], 1)

end