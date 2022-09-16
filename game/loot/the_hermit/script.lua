function TheHermit_effect(host)
    --  TODO? don't fizzle if not eniugh cards
    --  TODO? add ordering bottom cards
    local me = this(host)
    local deckType = TREASURE_DECK
    local owner = getTopOwner(host)
    local cards = topCardsOf(host, deckType, 5)
    if #cards ~= 5 then
        return
    end
    local choices = Common_FormCardChoices(cards)
    local posOf = function (choice_)
        for i, c in ipairs(choices) do
            if choice_ == c then
                return i
            end
        end
        return 0
    end
    local choice = requestSimpleChoice(host, owner.id, 'Place which card on top?', choices)
    local pos = posOf(choice)
    local card = cards[pos]
    putFromTopToBottom(host, deckType, 5)
    removeFromEverywhere(host, card.id)
    placeOnTop(host, deckType, card.id)
    discardMe(host, me.id)
end
