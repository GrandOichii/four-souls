--  TODO untested

function Succubus_cost(host, cardInfo)
    return Common_Pay(host, cardInfo.ownerID, 3)
end

function Succubus_succ(host)
    local owner = getTopOwner(host)
    addPlayableCount(host, owner.id)
    addPlayableCount(host, owner.id)
end