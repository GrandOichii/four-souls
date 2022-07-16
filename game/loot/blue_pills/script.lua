--  TODO untested

function BluePills_cost(host, cardInfo)
    return Common_Roll(host, cardInfo["ownerID"])
end

function BluePills_use(host, me)
    local roll = getLastRoll(host)["value"]
    local owner = getTopOwner(host)
    if roll == 1 or roll == 2 then
        lootCards(host, owner["id"], 1)
        return
    end
    if roll == 3 or roll == 4 then
        lootCards(host, owner["id"], 3)
        return
    end
    Common_Discard(host, owner["id"], 1)
end