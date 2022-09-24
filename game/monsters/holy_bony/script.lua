function HolyBony_rewards(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 2)
end

function HolyBony_trigger_check(host, me)
    return Common_LastRoll(host, me).value == 6
end

function HolyBony_trigger_cost(host, info)
    local me = this(host)
    local ownerID = CardData[me.id].lastRoll.ownerID
    local choice = requestSimpleChoice(host, ownerID, 'Use ability of ${'..me.key..'}?', {'Yes', 'No'})
    if choice == 'No' then
        --  TODO figure out if still uses stack
        return true
    end
    CardData[me.id].dChoice = requestSimpleChoice(host, ownerID, 'Choose deck', {TREASURE_DECK, LOOT_DECK, MONSTER_DECK})
    return true
end

function HolyBony_trigger(host)
    local me = this(host)
    local ownerID = CardData[me.id].lastRoll.ownerID
    Common_OrderTopCards(host, ownerID, CardData[me.id].dChoice, 3)
end
