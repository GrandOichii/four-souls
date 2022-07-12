function MomsCoinPurse_enter(host)

    local owner = getOwner(host)
    incBeginningLoot(host, owner["id"])

end

function MomsCoinPurse_leave(host)

    local owner = getOwner(host)
    decBeginningLoot(host, owner["id"])

end