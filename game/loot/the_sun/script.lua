function TheSun_use(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local current = getCurrentPlayer(host)
    if owner.id ~= current.id then 
        return
    end
    setNextPlayer(host, owner.id)
    placeOnTop(host, LOOT_DECK, me.id)
    putFromTopToBottom(host, LOOT_DECK, 1)
end