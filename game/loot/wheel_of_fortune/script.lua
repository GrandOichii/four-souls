function WheelOfFortune_cost(host, cardInfo)
    return Common_Roll(host, cardInfo["ownerID"])
end

function WheelOfFortune_use(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host)["value"]
    if roll == 1 then
        addCoins(host, owner["id"], 1)
        return
    end
    if roll == 2 then
        dealDamage(host, PLAYER, owner["id"], PLAYER, owner["id"], 2)
        return
    end
    if roll == 3 then
        lootCards(host, owner["id"], 3)
        return
    end
    if roll == 4 then
        local amount = owner["coins"]
        if amount > 4 then
            amount = 4
        end
        subCoins(host, owner["id"], amount)
        return
    end
    if roll == 5 then
        addCoins(host, owner["id"], 5)
        return
    end
    if roll == 6 then
        gainTreasure(host, owner.id, 1)
        return
    end
end