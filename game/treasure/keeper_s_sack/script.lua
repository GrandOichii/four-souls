function KeepersSack_enter(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 14)
end

function KeepersSack_trigger_check(host, me)
    return Common_OwnersTurn(host, me.id)
end

function KeepersSack_trigger(host)
    local owner = getTopOwner(host)
    if #owner.hand == 0 then
        dealDamage(host, PLAYER, owner.id, PLAYER, owner.id, 2)
        return
    end
    Common_Discard(host, owner.id, 1)
end

function KeepersSack_cost(host, info)
    return Common_TargetOpponent(host, info.ownerID)
end

function KeepersSack_tap(host)
    local me = this(host)
    local target = popTarget(host)
    removeFromEverywhere(host, me.id)
    moveToBoard(host, target.id, me.id)
end
