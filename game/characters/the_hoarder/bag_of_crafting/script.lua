function BagOfCrafting_cost1(host, info)
    return Common_Discard(host, info.ownerID, 1, true)
end

function BagOfCrafting_effect1(host)
    local me = this(host)
    addCounters(host, me.id, 1)
end

function BagOfCrafting_cost2(host, info)
    return Common_RemoveCounters(host, 4)
end

function BagOfCrafting_effect2(host)
    local owner = getTopOwner(host)
    gainTreasure(host, owner.id, 1)
end
