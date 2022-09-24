--  TODO untested

function MomsBox_trigger_check(host, me)
    return Common_LastRoll(host, me) == 4
end

function MomsBox_trigger(host)
    local owner = getTopOwner(host)
    local choice = requestSimpleChoice(host, owner.id, "Loot 1 and discard a loot card?", {"Yes", "No"})
    if choice == "No" then
        return
    end
    lootCards(host, owner.id, 1)
    Common_Discard(host, owner.id, 1, false)
end
