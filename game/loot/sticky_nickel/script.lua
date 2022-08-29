function StickyNickel_use(host)
    local me = this(host)
    local roll = popRollStack(host)
    local owner = getTopOwner(host)
    local players = getPlayers(host)
    if roll.value > 3 then
        addCoins(host, owner.id, 5)
        discardMe(host, me.id)
        return
    end
    local nextI = 0
    for i, player in ipairs(players) do
        if player.id == owner.id then
            nextI = i + 1
            break
        end
    end
    if nextI == #players + 1 then
        nextI = 1
    end
    moveToHand(host, players[nextI].id, me.id)
end