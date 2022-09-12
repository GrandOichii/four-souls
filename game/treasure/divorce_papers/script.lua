function DivorcePapers_cost(host, info)
    return Common_TargetOpponent(host, info.ownerID)
end

function DivorcePapers_effect(host)
    local target = popTarget(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local flag = Common_SacrificeMe(host, me.id)
    if not flag then
        return
    end
    local player = Common_PlayerWithID(host, target.id)
    -- loot
    local lAmount = math.floor(#player.hand)
    if lAmount ~= 0 then
        local cardIDs = requestCardsInHand(host, player.id, player.id, 'Choose a card to give to '..owner.name, lAmount)
        for _, id in ipairs(cardIDs) do
            removeFromEverywhere(host, id)
            moveToHand(host, owner.id, id)
        end
    end
    -- coins
    local cAmount = math.floor(player.coins)
    subCoins(host, player.id, cAmount)
    addCoins(host, owner.id, cAmount)
    -- item
    local ids = {}
    for _, card in ipairs(player.board) do
        if not card.isEternal then
            ids[#ids+1] = card.id
        end
    end
    if #ids == 0 then
        return
    end
    local choice, _ = requestChoice(host, player.id, 'Choose an item to give to '..owner.name, CARD, ids)
    removeFromEverywhere(host, choice)
    moveToBoard(host, owner.id, choice)
end
