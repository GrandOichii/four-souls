function Chaos_tap(host)
    local players = getPlayers(host)
    local cmap = {}
    for _, player in ipairs(players) do
        local newM = {
            pid = player.id,
            cids = {}
        }
        for _, card in ipairs(player.hand) do
            removeFromEverywhere(host, card.id)
            newM.cids[#newM.cids+1] = card.id
        end
        cmap[#cmap+1] = newM
    end
    for i, pmap in ipairs(cmap) do
        local nextI = i + 1
        if nextI == #cmap + 1 then
            nextI = 1
        end
        for _, cid in ipairs(pmap.cids) do
            moveToHand(host, cmap[nextI].pid, cid)
        end
    end
    -- for i, cards in ipairs(cmap) do
    --     local nextID = 0
    --     for
    -- end
end
