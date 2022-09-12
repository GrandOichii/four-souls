function TheSun_use(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local current = getCurrentPlayer(host)
    if owner.id ~= current.id then 
        return
    end
    for _, player in ipairs(getPlayers(host)) do
        if player.id ~= owner.id then
            incSkipCounter(host, player.id)
        end
    end
    placeOnTop(host, LOOT_DECK, me.id)
    putFromTopToBottom(host, LOOT_DECK, 1)
end