function HandMeDowns_tap(host)
    local owner = getTopOwner(host)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        if player.id ~= owner.id then
            local choice = requestSimpleChoice(host, player.id, 'Loot 1 and give '..owner.name..' 1 loot card?', {'Yes', 'No'})
            if choice == 'Yes' then
                lootCards(host, player.id, 1)
                local cardID = requestCardsInHand(host, player.id, player.id, 'Choose a card to give to '..owner.name, 1)[1]
                removeFromEverywhere(host, cardID)
                moveToHand(host, owner.id, cardID)
            end
        end
    end
end
