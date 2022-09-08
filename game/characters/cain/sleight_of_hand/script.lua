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
    local cards = topCardsOf(host, deckType, 5)
    if #cards ~= 5 then
        return
    end
    local choices = {}
    for i, card in ipairs(cards) do
        choices[#choices+1] = i..'- ${'..card.name..'}'
        removeFromEverywhere(host, card.id)
    end
    local posOf = function (choice_)
        for i, c in ipairs(choices) do
            if c == choice_ then
                return i
            end
        end
        return 0
    end
    while #choices ~= 0 do
        local choice = requestSimpleChoice(host, owner.id, 'Choose a card to put on top', choices)
        local pos = posOf(choice)
        table.remove(choices, pos)
        local card = cards[pos]
        table.remove(cards, pos)
        placeOnTop(host, deckType, card.id)
    end
end
