function Decoy_cost(host, me)
    local ids = {}
    local players = getPlayers(host)
    for _, player in ipairs(players) do
        if player.id ~= me.ownerID then
            for _, card in ipairs(player.board) do
                if not card.isEternal and card.id ~= me.id then
                    ids[#ids+1] = card.id
                end
            end
        end
    end
    if #ids == 0 then
        return false
    end
    local choiceId, _ = requestChoice(host, me.ownerID, "Choose a card", CARD, ids)
    pushTarget(host, choiceId, CARD)
    return true
end

function Decoy_tap(host)
    local me = this(host)
    local target = popTarget(host)
    local p = nil
    for _, player in ipairs(getPlayers(host)) do
        for _, card in ipairs(player.board) do
            if card.id == target.id then
                p = player
                break
            end
        end
    end
    if p == nil then
        return
    end
    local owner = getTopOwner(host)
    removeFromEverywhere(host, me.id)
    removeFromEverywhere(host, target.id)
    moveToBoard(host, owner.id, target.id)
    moveToBoard(host, p.id, me.id)
end
