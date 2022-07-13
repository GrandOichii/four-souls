function SackOfPennies_trigger_check(host, me)
    return Common_LastRoll(host)["value"] == 1
end

function SackOfPennies_trigger(host)

    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner["id"], "Recharge Sack of Pennies?", {"Yes", "No"})
    if (choice == "No") then
        return
    end
    local id = this(host)["id"]
    rechargeCard(host, id)

end

function SackOfPennies_cost(host, cardInfo)
    return Common_Tap(host)
end

function SackOfPennies_tap(host)

    local owner = getTopOwner(host)
    addCoins(host, owner["id"], 1)

end