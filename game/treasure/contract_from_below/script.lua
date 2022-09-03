function ContractFromBelow_cost(host, info)
    local owner = Common_PlayerWithID(host, info.ownerID)
    local ids = {}
    for _, card in ipairs(owner.board) do
        if not card.isEternal then
            ids[#ids+1] = card.id
        end
    end
    if #ids < 2 then
        return false
    end
    Common_SacrificeItem(host, info.ownerID)
    Common_SacrificeItem(host, info.ownerID)
    return Common_TargetOwnedNonEternalCard(host, info.ownerID)
end

function ContractFromBelow_effect(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    if not Common_CardOnBoard(host, target.id) then
        return false
    end
    removeFromEverywhere(host, target.id)
    moveToBoard(host, owner.id, target.id)
end
