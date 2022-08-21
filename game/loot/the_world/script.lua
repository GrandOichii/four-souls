function TheWorld_use(host)
    local owner = getTopOwner(host)
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        if player.id ~= owner.id then
            if #player.hand ~= 0 then
                requestCardsInHand(host, owner.id, player.id, 'Looking at '..player.name..'\'s hand. Click any card to continue', 1)
            end
        end
    end
    lootCards(host, owner.id, 2)
    discardMe(host, this(host).id)

end