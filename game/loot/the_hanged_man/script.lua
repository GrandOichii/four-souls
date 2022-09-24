--  TODO untested

function TheHangedMan_use(host)
    discardMe(host, this(host).id)
    local owner = getTopOwner(host)
    local decks = {LOOT_DECK, TREASURE_DECK, MONSTER_DECK}
    for _, deck in ipairs(decks) do
        local cards = topCardsOf(host, deck, 1)
        if #cards ~= 0 then
            local card = cards[1]
            local choice = requestSimpleChoice(host, owner.id, 'Put ${'..card.key..'} on the bottom of the '..deck..' deck?', {'Yes', 'No'})
            if choice == 'Yes' then
                putFromTopToBottom(host, deck, 1)
            end
        end
    end
    lootCards(host, owner.id, 2)
    

end