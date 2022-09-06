function Eden_use(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end

function Eden_game_start(host)
    local owner = getTopOwner(host)
    local cards = topCardsOf(host, TREASURE_DECK, 3)
    local message = 'Choose:'
    local choices = {}
    for i, card in ipairs(cards) do
        message = message..'${'..card.name..'}, '
        choices[#choices+1] = i..'- ${'..card.name..'}'
    end
    local choice = requestSimpleChoice(host, owner.id, message, choices)
    putFromTopToBottom(host, TREASURE_DECK, 3)
    local choiceID = -1
    for i, c in ipairs(choices) do
        if choice == c then
            choiceID = cards[i].id
            break
        end
    end
    removeFromEverywhere(host, choiceID)
    moveToBoard(host, owner.id, choiceID)
    setIsEternal(host, choiceID, true)
end