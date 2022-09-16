function FetalHaunt_trigger1_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function FetalHaunt_trigger1_cost(host, info)
    return Common_TargetOpponent(host, info.ownerID)
end

function FetalHaunt_trigger1(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    local owner = getTopOwner(host)
    if #owner.hand < 0 then
        return false
    end
    local ids = {}
    for _, card in ipairs(owner.hand) do
        ids[#ids+1] = card.id
    end
    if #ids == 0 then
        return
    end
    local choices = requestCardsInHand(host, owner.id, owner.id, 'Choose a card to '..player.name, 1)
    local choice = choices[1]
    removeFromEverywhere(host, choice)
    moveToHand(host, player.id, choice)
end