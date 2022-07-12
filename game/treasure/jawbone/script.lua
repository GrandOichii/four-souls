function Jawbone_cost(host, cardInfo)
    return Common_TargetOpponent(host, cardInfo) and Common_Tap(host, cardInfo)
end

function Jawbone_tap(host)
    local owner = getOwner(host)
    local target = popTarget(host)
    local opponent = Common_PlayerWithID(host, target["id"])
    local coins = opponent["coins"]
    local amount = math.min(3, coins)
    subCoins(host, opponent["id"], amount)
    addCoins(host, owner["id"], amount)
end