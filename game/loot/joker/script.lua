function Joker_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function Joker_use(host)
    discardMe(host, this(host).id)

    local owner = getTopOwner(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    if #player.hand == 0 then
        return
    end
    local cid = requestCardsInHand(host, owner.id, player.id, 'Choose a card to steal', 1)[1]
    removeFromEverywhere(host, cid)
    moveToHand(host, owner.id, cid)
end
