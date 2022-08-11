function GuppysHead_cost(host, cardInfo)
    return Common_Tap(host) and Common_TargetPlayer(host, cardInfo)
end

function GuppysHead_tap(host)
    local target = popTarget(host)
    local owner = getTopOwner(host)
    local player = Common_PlayerWithID(host, target.id)
    if #player.hand == 0 then
        return
    end
    local cardID = requestCardsInHand(host, owner.id, target.id, "Choose a card to give to "..owner.id, 1)[1]
    removeFromEverywhere(host, cardID)
    moveToHand(host, owner.id, cardID)
end
