local BAD_EDEN_CARDS = {
    'Experimental Treatment',
    'Forget Me Now',
    'R Key',
    'The Chest',
    'Pageant Boy',
    'Pandora\'s Box',
    'Glass Cannon',
    'The D4',
    'Mom\'s Shovel',
    'Plan C',
    'Mama Mega',
    'Chaos Card',
    'Eden\'s Soul',
    '1-UP',
}

function IsBadEdenCard(cardName)
    for _, name in ipairs(BAD_EDEN_CARDS) do
        if name == cardName then
            return true
        end
    end
    return false
end

local EDEN_CHOICE_AMOUNT = 3

function Eden_use(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
end

function Eden_game_start(host)
    local owner = getTopOwner(host)
    local message = 'Choose:'
    local choices = {}
    local cards = {}
    local count = 0
    while count ~= EDEN_CHOICE_AMOUNT do
        local tc = topCardsOf(host, TREASURE_DECK, 1)[1]
        if not IsBadEdenCard(tc.name) then
            count = count + 1
            cards[#cards+1] = tc
            choices[#choices+1] = count..'- ${'..tc.name..'}'
        end
        putFromTopToBottom(host, TREASURE_DECK, 1)
    end
    local choice = requestSimpleChoice(host, owner.id, message, choices)
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