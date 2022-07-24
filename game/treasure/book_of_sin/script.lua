function BookOfSin_cost(host, cardInfo)
    return Common_Tap(host) and Common_Roll(host, cardInfo["ownerID"])
end

function BookOfSin_tap(host)
    local me = this(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host)["value"]
    if roll == 1 or roll == 2 then
        addCoins(host, owner["id"], 1)
    end
    if roll == 3 or roll == 4 then
        lootCards(host, owner["id"], 1)
    end
    if roll == 5 or roll == 6 then
        --  TODO untested
        Common_TempIncMaxLife(host, me.id, owner.id, 1)
    end
end