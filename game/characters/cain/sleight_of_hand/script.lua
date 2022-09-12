function SleightOfHand_cost(host, info)
    local me = this(host)
    local choice = requestSimpleChoice(host, info.ownerID, 'Choose a deck', {TREASURE_DECK, LOOT_DECK, MONSTER_DECK})
    CardData[me.id] = {
        choice = choice
    }
    return true
end

function SleightOfHand_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local deckType = CardData[me.id].choice
    Common_OrderTopCards(host, owner.id, deckType, 5)
end
