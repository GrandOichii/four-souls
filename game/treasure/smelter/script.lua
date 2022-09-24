function Smelter_cost(host, cardInfo)
    return Common_Discard(host, cardInfo.ownerID, 1, true)
end

function Smelter_smelt(host)
    local owner = getTopOwner(host)
    addCoins(host, owner.id, 3)
end