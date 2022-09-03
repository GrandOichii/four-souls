function TheDevil_cost(host, info)
    return Common_TargetNonEternalCard(host, info.ownerID)
end

function TheDevil_effect(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    local ids = {}
    for _, card in ipairs(owner.board) do
        if not card.isEternal then
            ids[#ids+1] = card.id
        end
    end
    if #ids == 0 then
        return
    end
    local choice, _ = requestChoice(host, owner.id, 'Choose an item to sacrifice', CARD, ids)
    destroyCard(host, choice)
    for _, card in ipairs(getShop(host)) do
        if card.id == target.id then
            removeFromEverywhere(host, target.id)
            moveToBoard(host, owner.id, target.id)
            pushRefillShop(host)
            return
        end
    end
    for _, player in ipairs(getPlayers(host)) do
        for _, card in ipairs(player.board) do
            if card.id == target.id then
                removeFromEverywhere(host, target.id)
                moveToBoard(host, owner.id, target.id)
                return
            end
        end
    end
end
