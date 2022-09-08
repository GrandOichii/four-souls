function EdensSoul_enter(host)
    local me = this(host)
    tapCard(host, me.id)
end

function EdensSoul_tap(host)
    local owner = getTopOwner(host)
    local me = this(host)
    local done = Common_SacrificeMe(host, me.id)
    if not done then
        return
    end
    --  TODO? don't fizzle if not enough cards
    local cards = topCardsOf(host, TREASURE_DECK, 7)
    if #cards ~= 7 then
        return
    end
    local choices = Common_FormCardChoices(cards)
    local posOf = function (choice_)
        for i, c in ipairs(choices) do
            if c == choice_ then
                return i
            end
        end
        return 0
    end
    for _ = 1, 2 do
        local choice = requestSimpleChoice(host, owner.id, 'Choose an item to gain', choices)
        local pos = posOf(choice)
        local card = cards[pos]
        table.remove(cards, pos)
        table.remove(choices, pos)
        removeFromEverywhere(host, card.id)
        moveToBoard(host, owner.id, card.id)
    end
    putFromTopToBottom(host, TREASURE_DECK, 5)
end
