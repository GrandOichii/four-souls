function Boomerang_cost(host, cardInfo)
    return Common_TargetOpponent(host, cardInfo.ownerID)
end

function Boomerang_tap(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    if #player.hand == 0 then
        return
    end
    local cid = player.hand[math.random(1, #player.hand)].id
    removeFromEverywhere(host, cid)
    moveToHand(host, owner.id, cid)
end
