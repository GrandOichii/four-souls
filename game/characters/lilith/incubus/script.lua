function Incubus_cost1(host, info)
    return Common_TargetOpponent(host, info.ownerID)
end

function Incubus_effect1(host)
    --  TODO fix when implemented guppy's eye
    local owner = getTopOwner(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    if #player.hand == 0 then
        return
    end
    requestCardsInHand(host, owner.id, player.id, player.name..'\'s hand', 1)
    if #owner.hand == 0 then
        return
    end
    local choice = requestSimpleChoice(host, owner.id, 'Swap cards with '..player.name..'?', {'Yes', 'No'})
    if choice == 'No' then
        return
    end
    local cards = requestCardsInHand(host, owner.id, player.id, 'Choose a card to swap', 1)
    local oCard = cards[1]
    cards = requestCardsInHand(host, owner.id, owner.id, 'Choose a card to swap', 1)
    local card = cards[1]
    removeFromEverywhere(host, oCard.id)
    removeFromEverywhere(host, card.id)
    moveToHand(host, owner.id, oCard.id)
    moveToHand(host, player.id, card.id)
end

function Incubus_effect2(host)
    local owner = getTopOwner(host)
    lootCards(host, owner.id, 1)
    owner = Common_PlayerWithID(host, owner.id)
    local message = "Choose a card to place on top of the loot deck"
    if #owner.hand == 0 then
        return
    end
    local result = requestCardsInHand(host, owner.id, owner.id, message, 1)[1]
    removeFromEverywhere(host, result)
    placeOnTop(host, LOOT_DECK, result)
end
