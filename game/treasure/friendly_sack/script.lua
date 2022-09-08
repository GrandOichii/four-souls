function FriendlySack_cost(host, info)
    return Common_TargetOpponent(host, info.ownerID)
end

function FriendlySack_effect(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    local cards = topCardsOf(host, LOOT_DECK, 3)
    if #cards ~= 3 then
        return
    end
    for _, card in ipairs(cards) do
        removeFromEverywhere(host, card.id)
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
    local choice = requestSimpleChoice(host, owner.id, 'Pick a card to take', choices)
    local pos = posOf(choice)
    local card = cards[pos]
    table.remove(cards, pos)
    moveToHand(host, owner.id, card.id)
    for _, c in ipairs(cards) do
        moveToHand(host, target.id, c.id)
    end
end
