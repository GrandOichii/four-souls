function SpelunkerHat_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function SpelunkerHat_trigger_cost(host, info)
    local me = this(host)
    local choice = requestSimpleChoice(host, info.ownerID, 'Use effect of ${'..me.name..'}?', {'Yes', 'No'})
    if choice == 'No' then
        return false
    end
    choice = requestSimpleChoice(host, info.ownerID, 'Choose deck', {TREASURE_DECK, LOOT_DECK, MONSTER_DECK})
    CardData[me.id] = {
        dChoice = choice
    }
    return true
end

function SpelunkerHat_trigger(host)
    local me = this(host)
    local owner = getTopOwner(host)
    Common_OrderTopCards(host, owner.id, CardData[me.id].dChoice, 3)
end
