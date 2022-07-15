--  TODO untested
--  TODO replace this with choice stack popping

function RainbowBaby_cost(host, cardInfo)
    return Common_Tap(host)
end

function RainbowBaby_tap(host)
    local owner = getTopOwner(host)
    local choice1 = "All players loot 1."
    local choice2 = "All players gain 2c."
    local choice3 = "All players take 1 damage."
    local choice = requestSimpleChoice(host, owner["id"], "Choose:", {choice1, choice2, choice3})
    if choice == choice1 then
        Common_AllPlayersLoot(host, 1)
        return
    end
    if choice == choice2 then
        Common_AllPlayersAddCoins(host, 2)
        return
    end
    if choice == choice3 then
        Common_DamageAllPlayers(host, 1)
        return
    end
end