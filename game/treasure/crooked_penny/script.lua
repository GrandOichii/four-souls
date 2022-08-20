--  TODO untested

function CrookedPenny_cost(host, cardInfo)
    return Common_TargetPlayer(host, cardInfo.ownerID)
end

function CrookedPenny_tap(host)
    local target = popTarget(host)
    local player = Common_PlayerWithID(host, target.id)
    local roll = popRollStack(host).value
    if roll < 4 then
        addCoins(host, player.id, player.coins)
        return
    end
    subCoins(host, player.id, player.coins)
end