function MomsPurse_enter(host)

    local owner = getOwner(host)
    incBeginningLoot(host, owner["id"], 1)

end

function MomsPurse_leave(host)

    local owner = getOwner(host)
    decBeginningLoot(host, owner["id"], 1)

end