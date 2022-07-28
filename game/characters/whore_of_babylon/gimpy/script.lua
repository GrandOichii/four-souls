--  TODO untested

function Gimpy_check(host, me)
    return Common_OwnerDamaged(host, me.id)
end

function Gimpy_effect(host)
    local _ = getDamageEvent(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local choice1 = "Gain +2a till end of turn"
    local choice2 = "Gain 2c"
    local choice3 = "Loot 1, then discard a loot card"
    local choice = requestSimpleChoice(host, owner.id, "Choose 1:", {choice1, choice2, choice3})
    if choice == choice1 then
        Common_TempIncAttack(host, me.id, owner.id, 2)
        return
    end
    if choice == choice2 then
        addCoins(host, owner.id, 2)
        return
    end
    if choice == choice3 then
        lootCards(host, owner.id, 1)
        Common_Discard(host, owner.id, 1)
        return
    end
end