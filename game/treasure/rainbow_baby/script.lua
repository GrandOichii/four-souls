local RainbowBaby_choice1 = "All players loot 1."
local RainbowBaby_choice2 = "All players gain 2c."
local RainbowBaby_choice3 = "All players take 1 damage."

function RainbowBaby_cost(host, cardInfo)
    local tapped = Common_Tap(host)
    if not tapped then
        return false
    end
    local choice = requestSimpleChoice(host, cardInfo.ownerID, "Choose:", {RainbowBaby_choice1, RainbowBaby_choice2, RainbowBaby_choice3})
    CardData[this(host).id] = {
        choice = choice
    }
    return true
end

function RainbowBaby_tap(host)
    local choice = CardData[this(host).id].choice
    if choice == RainbowBaby_choice1 then
        Common_AllPlayersLoot(host, 1)
        return
    end
    if choice == RainbowBaby_choice2 then
        Common_AllPlayersAddCoins(host, 2)
        return
    end
    if choice == RainbowBaby_choice3 then
        Common_DamageAllPlayers(host, 1)
        return
    end
end