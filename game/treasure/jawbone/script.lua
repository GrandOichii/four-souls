function Jawbone_cost(host, cardInfo)
    return Common_TargetOpponent(host, cardInfo.ownerID)
end

function Jawbone_tap(host)
    local owner = getTopOwner(host)
    local target = popTarget(host)
    local opponent = Common_PlayerWithID(host, target.id)
    local coins = opponent.coins
    local amount = math.min(3, coins)
    subCoins(host, opponent.id, amount)
    addCoins(host, owner.id, amount)
end