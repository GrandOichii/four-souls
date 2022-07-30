function TheWorld_use(host)
    local owner = getTopOwner(host)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        if player.id ~= owner.id then
            if #player.hand ~= 0 then
                print('HAND SIZE: '..#player.hand)
                requestCardsInHand(host, owner.id, player.id, 'Looking at '..player.name..'\'s hand (click any card to continue)', 1)
            end
        end
    end
    lootCards(host, owner.id, 2)
end