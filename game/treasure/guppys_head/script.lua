function GuppysHead_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function GuppysHead_tap(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    local player = Common_PlayerWithID(host, target.id)
    if #player.hand == 0 then
        return
    end
    local cardID = requestCardsInHand(host, target.id, target.id, "Choose a card to give to "..owner.name, 1)[1]
    removeFromEverywhere(host, cardID)
    moveToHand(host, owner.id, cardID)
end
