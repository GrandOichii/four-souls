function BagOfCrafting_cost(host, cardInfo)
    return Common_Discard(host, cardInfo["ownerID"], 4)
end

function BagOfCrafting_craft(host)
    local owner = getTopOwner(host)
    plusOneTreasure(host, owner["id"])
end