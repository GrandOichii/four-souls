function D100_cost(host, cardInfo)
    return Common_Tap(host) and Common_Roll(host, cardInfo["ownerID"])
end

function D100_tap(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host)["value"]
    if roll == 1 then
        lootCards(host, owner["id"], 1)
    end
    if roll == 2 then
        lootCards(host, owner["id"], 2)
    end
    if roll == 3 then
        addCoins(host, owner["id"], 3)
    end
    if roll == 4 then
        addCoins(host, owner["id"], 4)
    end
    if roll == 5 then
        tempIncMaxLife(host, owner["id"], 1)
    end
    if roll == 6 then
        tempIncAttack(host, owner["id"], 1)
    end
end