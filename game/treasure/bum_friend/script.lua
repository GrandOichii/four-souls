function BumFriend_effect(host)
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