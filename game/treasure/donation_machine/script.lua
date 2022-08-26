function DonationMachine_cost(host, me)
    local owner = getTopOwner(host)
    local ids = {}
    for _, card in ipairs(owner.board) do
        if not card.isEternal and card.id ~= me.id then
            ids[#ids+1] = card.id
        end
    end
    if #ids == 0 then
        return false
    end
    local choice, _ = requestChoice(host, owner.id, 'Choose an item to donate', CARD, ids)
    pushTarget(host, choice, CARD)
    return Common_TargetOpponent(host, owner.id)
end

function DonationMachine_effect(host)
    local pTarget = popTarget(host)
    local cTarget = popTarget(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 8)
    removeFromEverywhere(host, cTarget.id)
    moveToBoard(host, pTarget.id, cTarget.id)
end
