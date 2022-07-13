function BookOfSin_cost(host, cardInfo)
    return Common_Tap(host, cardInfo) and Common_Roll(host, cardInfo)
end

function BookOfSin_tap(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host)
    if roll == 1 or roll == 2 then
        addCoins(host, owner["id"], 1)
    end
    if roll == 3 or roll == 4 then
        lootCards(host, owner["id"], 1)
    end
    if roll == 5 or roll == 6 then
        tempIncMaxLife(host, owner["id"], 1)
    end
end