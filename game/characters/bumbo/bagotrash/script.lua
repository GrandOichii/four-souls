--  TODO move choice to choice stack

function Bagotrash_cost(host, cardInfo)
    return Common_Pay(host, cardInfo["ownerID"], 4)
end

function Bagotrash_pay(host)
    local owner = getTopOwner(host)
    local choice1 = "Loot 1"
    local choice2 = "Deal 1 damage to a monster"
    local choice3 = "Deal 1 damage to a player"
    local choice4 = "Play an additional loot card this turn"
    local choice = requestSimpleChoice(host, owner["id"], "Choose 1:", {choice1, choice2, choice3, choice4})
    if choice == choice1 then
        lootCards(host, owner["id"], 1)
        return
    end
    if choice == choice2 then
        local monster = Common_ChooseMonster(host, owner["id"])
        dealDamage(host, PLAYER, owner["id"], MONSTER, monster["id"], 1)
        return
    end
    if choice == choice3 then
        local target = Common_ChoosePlayer(host, owner["id"])
        dealDamage(host, PLAYER, owner["id"], PLAYER, target["id"], 1)
        return
    end
    addPlayableCount(host, owner["id"])
end