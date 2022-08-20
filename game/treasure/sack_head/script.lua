function SackHead_cost(host, cardInfo)
    --  TODO move choice to cost
    return true
end

function SackHead_tap(host)
    local owner = getTopOwner(host)
    local choices = {LOOT_DECK, TREASURE_DECK, MONSTER_DECK}
    local deckType = requestSimpleChoice(host, owner.id, 'Look at the top of which deck?', choices)
    local cards = topCardsOf(host, deckType, 1)
    if #cards == 0 then
        return
    end
    local card = cards[1]
    local choice = requestSimpleChoice(host, owner.id, 'Put ${'..card.name..'} on the bottom of the '..deckType..' deck?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    putFromTopToBottom(host, deckType, 1)
end