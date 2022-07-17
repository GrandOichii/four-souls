function RedPills_cost(host, cardInfo)
    return Common_Roll(host, cardInfo["ownerID"])
end

function RedPills_use(host)
    local owner = getTopOwner(host)
    local roll = getLastRoll(host)["value"]
    if roll == 1 or roll == 2 then
        tempIncAttack(host, owner["id"], 1)
        return
    end
    if roll == 3 or roll == 4 then
        tempIncMaxLife(host, owner["id"], 1)
        return
    end
    if roll == 5 or roll == 6 then
        dealDamage(host, PLAYER, owner["id"], PLAYER, owner["id"], 1)
        return
    end
end